#include "main.h"
#include "stm32f1xx_hal_gpio_ex.h"

#include "input_reading.h"
#include "input_processing.h"
#include "output_display.h"

TIM_HandleTypeDef htim2;

#define SCAN_MS        10U
#define BLINK_HALF_MS 250U
#define CONFIRM_MS    500U

/*
 * TRAFFIC PHASES
 *
 * 0 = NS GREEN,  EW RED
 * 1 = NS YELLOW, EW RED
 * 2 = BOTH RED (gap1)
 * 3 = NS RED,    EW GREEN
 * 4 = NS RED,    EW YELLOW
 * 5 = BOTH RED (gap2)
 */

static uint8_t traffic_phase = 5U;
static uint32_t traffic_start_ms = 0U;

void SystemClock_Config(void);

static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* =========================================================
   TRAFFIC LED DRIVER
   ========================================================= */

static void traffic_all_off(void)
{
    HAL_GPIO_WritePin(
        RED_NS_GPIO_Port, RED_NS_Pin, GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        YELLOW_NS_GPIO_Port, YELLOW_NS_Pin, GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GREEN_NS_GPIO_Port, GREEN_NS_Pin, GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        RED_EW_GPIO_Port, RED_EW_Pin, GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        YELLOW_EW_GPIO_Port, YELLOW_EW_Pin, GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GREEN_EW_GPIO_Port, GREEN_EW_Pin, GPIO_PIN_SET
    );
}

/* Apply one NORMAL traffic phase */
static void traffic_set_normal(uint8_t phase)
{
    traffic_all_off();

    switch (phase) {
        case 0:
            /* NS GREEN, EW RED */
            HAL_GPIO_WritePin(
                GREEN_NS_GPIO_Port, GREEN_NS_Pin,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                RED_EW_GPIO_Port, RED_EW_Pin,
                GPIO_PIN_RESET
            );
            break;

        case 1:
            /* NS YELLOW, EW RED */
            HAL_GPIO_WritePin(
                YELLOW_NS_GPIO_Port, YELLOW_NS_Pin,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                RED_EW_GPIO_Port, RED_EW_Pin,
                GPIO_PIN_RESET
            );
            break;

        case 2:
        case 5:
            /* Both directions RED */
            HAL_GPIO_WritePin(
                RED_NS_GPIO_Port, RED_NS_Pin,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                RED_EW_GPIO_Port, RED_EW_Pin,
                GPIO_PIN_RESET
            );
            break;

        case 3:
            /* NS RED, EW GREEN */
            HAL_GPIO_WritePin(
                RED_NS_GPIO_Port, RED_NS_Pin,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                GREEN_EW_GPIO_Port, GREEN_EW_Pin,
                GPIO_PIN_RESET
            );
            break;

        case 4:
            /* NS RED, EW YELLOW */
            HAL_GPIO_WritePin(
                RED_NS_GPIO_Port, RED_NS_Pin,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                YELLOW_EW_GPIO_Port, YELLOW_EW_Pin,
                GPIO_PIN_RESET
            );
            break;

        default:
            break;
    }
}

/* =========================================================
   CALCULATE TRAFFIC DURATIONS

   Red duration must be >= Green + Yellow.

   If configured RED is larger, add two
   both-red clearance intervals.

   Effective RED = max(RED, GREEN + YELLOW)
   ========================================================= */

static void traffic_durations(
    uint8_t *g,
    uint8_t *y,
    uint16_t *gap1,
    uint16_t *gap2
)
{
    uint16_t r = getRedTime();

    *g = getGreenTime();
    *y = getAmberTime();

    uint16_t sum = (uint16_t)(*g + *y);

    uint16_t extra =
        (r > sum) ? (uint16_t)(r - sum) : 0U;

    *gap1 = extra / 2U;
    *gap2 = extra - *gap1;
}

/* Get duration of one phase, in milliseconds */
static uint32_t phase_duration_ms(
    uint8_t phase,
    uint8_t g,
    uint8_t y,
    uint16_t gap1,
    uint16_t gap2
)
{
    switch (phase) {
        case 0:
        case 3:
            return (uint32_t)g * 1000U;

        case 1:
        case 4:
            return (uint32_t)y * 1000U;

        case 2:
            return (uint32_t)gap1 * 1000U;

        case 5:
            return (uint32_t)gap2 * 1000U;

        default:
            return 1000U;
    }
}

/* Four 7-segment digits can display at most 99 per road */
static uint8_t limit_digit_pair(uint32_t n)
{
    return (n > 99U) ? 99U : (uint8_t)n;
}

/* =========================================================
   MODE 1 - NORMAL TRAFFIC FSM

   Runs without HAL_Delay.
   ========================================================= */

static void normal_mode_update(uint32_t now)
{
    uint8_t green, yellow;
    uint16_t gap1, gap2;

    traffic_durations(
        &green, &yellow, &gap1, &gap2
    );

    uint32_t duration = phase_duration_ms(
        traffic_phase, green, yellow, gap1, gap2
    );

    /*
     * Advance when the current phase expires.
     * Zero-duration clearance phases are skipped.
     */
    while (duration == 0U ||
           (uint32_t)(now - traffic_start_ms) >= duration) {

        traffic_start_ms += duration;

        traffic_phase =
            (uint8_t)((traffic_phase + 1U) % 6U);

        traffic_set_normal(traffic_phase);

        duration = phase_duration_ms(
            traffic_phase, green, yellow, gap1, gap2
        );
    }

    /* Remaining seconds in current phase */
    uint32_t remaining =
        (duration -
        (uint32_t)(now - traffic_start_ms) + 999U)
        / 1000U;

    uint32_t ns = 0U;
    uint32_t ew = 0U;

    /*
     * Show countdown for the CURRENT color
     * of each direction.
     */
    switch (traffic_phase) {
        case 0:
            ns = remaining;
            ew = remaining + yellow + gap1;
            break;

        case 1:
            ns = remaining;
            ew = remaining + gap1;
            break;

        case 2:
            ns = remaining + green + yellow + gap2;
            ew = remaining;
            break;

        case 3:
            ns = remaining + yellow + gap2;
            ew = remaining;
            break;

        case 4:
            ns = remaining + gap2;
            ew = remaining;
            break;

        case 5:
            ns = remaining;
            ew = remaining + green + yellow + gap1;
            break;

        default:
            break;
    }

    /* First two digits = EW, last two digits = NS */
    display_show_values(
        limit_digit_pair(ew),
        limit_digit_pair(ns)
    );
}

/* =========================================================
   MODIFICATION MODE - 2Hz BLINK

   ON 250ms, OFF 250ms
   ========================================================= */

static void traffic_blink(uint8_t mode, uint8_t on)
{
    traffic_all_off();

    if (!on) {
        return;
    }

    switch (mode) {
        case MODE_2_RED:
            HAL_GPIO_WritePin(
                RED_NS_GPIO_Port, RED_NS_Pin,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                RED_EW_GPIO_Port, RED_EW_Pin,
                GPIO_PIN_RESET
            );
            break;

        case MODE_3_YELLOW:
            HAL_GPIO_WritePin(
                YELLOW_NS_GPIO_Port, YELLOW_NS_Pin,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                YELLOW_EW_GPIO_Port, YELLOW_EW_Pin,
                GPIO_PIN_RESET
            );
            break;

        case MODE_4_GREEN:
            HAL_GPIO_WritePin(
                GREEN_NS_GPIO_Port, GREEN_NS_Pin,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                GREEN_EW_GPIO_Port, GREEN_EW_Pin,
                GPIO_PIN_RESET
            );
            break;

        default:
            break;
    }
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    HAL_Init();

    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();

    button_init();
    display_init();

    uint32_t now = HAL_GetTick();

    uint32_t last_scan_ms = now;
    uint32_t mode_start_ms = now;
    uint32_t saved_at_ms = now;

    uint32_t last_blink_half = UINT32_MAX;

    uint8_t previous_mode = 0U;
    uint8_t last_save_sequence = getSaveSequence();
    uint8_t confirm_save = 0U;

    traffic_all_off();

    /* TIM2: interrupt every 10 ms */
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK) {
        Error_Handler();
    }

    while (1) {
        /* 1. Read button events */
        fsm_for_input_processing();

        now = HAL_GetTick();

        uint8_t mode = getCurrentMode();

        /* 2. Handle mode transition */
        if (mode != previous_mode) {
            previous_mode = mode;
            mode_start_ms = now;
            last_blink_half = UINT32_MAX;
            confirm_save = 0U;

            traffic_all_off();

            if (mode == MODE_1_NORMAL) {
                /*
                 * Start with the last both-red phase.
                 * If its duration is zero, it is
                 * skipped immediately.
                 */
                traffic_phase = 5U;
                traffic_start_ms = now;

                traffic_set_normal(traffic_phase);
            }
        }

        /* 3. Detect successful BT3 save */
        uint8_t seq = getSaveSequence();

        if (seq != last_save_sequence) {
            last_save_sequence = seq;
            saved_at_ms = now;
            confirm_save = 1U;
        }

        /* 4. Normal mode */
        if (mode == MODE_1_NORMAL) {
            normal_mode_update(now);
        }
        else {
            /* 5. Modification mode display */
            if (confirm_save &&
                (uint32_t)(now - saved_at_ms) < CONFIRM_MS) {

                /*
                 * Example:
                 * Saved value = 08 -> 0888
                 */
                display_show_values(
                    getSavedTimeForMode(mode),
                    88U
                );
            }
            else {
                confirm_save = 0U;

                /*
                 * Mode 2: 0802
                 * Mode 3: 0403
                 * Mode 4: 0604
                 */
                display_show_values(
                    getCurrentEditValue(),
                    mode
                );
            }

            /* 6. Continuous blinking */
            uint32_t half =
                (uint32_t)(now - mode_start_ms)
                / BLINK_HALF_MS;

            if (half != last_blink_half) {
                traffic_blink(
                    mode,
                    (uint8_t)((half & 1U) == 0U)
                );

                last_blink_half = half;
            }
        }

        /* 7. Scan one digit every 2 ms */
        if ((uint32_t)(now - last_scan_ms) >= SCAN_MS) {
            last_scan_ms = now;
            display_scan_next();
        }
    }
}

/* =========================================================
   SYSTEM CLOCK - HSI 8 MHz
   ========================================================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    osc.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&osc) != HAL_OK) {
        Error_Handler();
    }

    clk.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(
        &clk, FLASH_LATENCY_0
    ) != HAL_OK) {
        Error_Handler();
    }
}

/* =========================================================
   TIM2 - 10ms interrupt
   ========================================================= */

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef clock = {0};
    TIM_MasterConfigTypeDef master = {0};

    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7999U;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 9U;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    htim2.Init.AutoReloadPreload =
        TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }

    clock.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    if (HAL_TIM_ConfigClockSource(
        &htim2, &clock
    ) != HAL_OK) {
        Error_Handler();
    }

    master.MasterOutputTrigger = TIM_TRGO_RESET;
    master.MasterSlaveMode =
        TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(
        &htim2, &master
    ) != HAL_OK) {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(TIM2_IRQn, 1U, 0U);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/* =========================================================
   GPIO
   ========================================================= */

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef init = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    /*
     * PA13/PA14 used as buttons in Proteus.
     * This disables SWD/JTAG.
     */
    __HAL_AFIO_REMAP_SWJ_DISABLE();

    /* Initial output levels: HIGH = OFF */
    HAL_GPIO_WritePin(
        GPIOA,
        SEG_A_Pin | SEG_B_Pin |
        SEG_C_Pin | SEG_D_Pin |
        SEG_E_Pin | SEG_F_Pin |
        SEG_G_Pin | RED_NS_Pin |
        EN0_Pin | EN1_Pin |
        EN2_Pin | EN3_Pin,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        YELLOW_NS_Pin |
        GREEN_NS_Pin |
        RED_EW_Pin |
        YELLOW_EW_Pin |
        GREEN_EW_Pin,
        GPIO_PIN_SET
    );

    /* GPIOA outputs */
    init.Pin =
        SEG_A_Pin | SEG_B_Pin |
        SEG_C_Pin | SEG_D_Pin |
        SEG_E_Pin | SEG_F_Pin |
        SEG_G_Pin | RED_NS_Pin |
        EN0_Pin | EN1_Pin |
        EN2_Pin | EN3_Pin;

    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &init);

    /* GPIOB traffic LEDs */
    init.Pin =
        YELLOW_NS_Pin |
        GREEN_NS_Pin |
        RED_EW_Pin |
        YELLOW_EW_Pin |
        GREEN_EW_Pin;

    HAL_GPIO_Init(GPIOB, &init);

    /* Buttons: BT1 PA12, BT2 PA13, BT3 PA14 */
    init.Pin = BT1_Pin | BT2_Pin | BT3_Pin;
    init.Mode = GPIO_MODE_INPUT;
    init.Pull = GPIO_PULLUP;

    HAL_GPIO_Init(GPIOA, &init);
}

/* =========================================================
   TIM2 INTERRUPT CALLBACK
   ========================================================= */

void HAL_TIM_PeriodElapsedCallback(
    TIM_HandleTypeDef *htim
)
{
    if (htim->Instance == TIM2) {
        button_reading();
    }
}

/* =========================================================
   ERROR HANDLER
   ========================================================= */

void Error_Handler(void)
{
    __disable_irq();

    while (1) {
    }
}
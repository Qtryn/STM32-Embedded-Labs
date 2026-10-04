/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
/* Matrix animation timer */
int timerAnimation_counter = 0;
int timerAnimation_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
void setTimerAnimation(int duration);
void timerAnimation_run(void);
void shiftMatrixLeft(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* =========================================================
   7-SEGMENT DISPLAY

   Common Anode

   SEG = 0 -> ON
   SEG = 1 -> OFF
   ========================================================= */

void display7SEG(uint8_t num)
{
    /*
     * SEG0 = A
     * SEG1 = B
     * SEG2 = C
     * SEG3 = D
     * SEG4 = E
     * SEG5 = F
     * SEG6 = G
     */

    uint8_t seg[10][7] =
    {
        /* A  B  C  D  E  F  G */

        {0, 0, 0, 0, 0, 0, 1},   // 0
        {1, 0, 0, 1, 1, 1, 1},   // 1
        {0, 0, 1, 0, 0, 1, 0},   // 2
        {0, 0, 0, 0, 1, 1, 0},   // 3
        {1, 0, 0, 1, 1, 0, 0},   // 4
        {0, 1, 0, 0, 1, 0, 0},   // 5
        {0, 1, 0, 0, 0, 0, 0},   // 6
        {0, 0, 0, 1, 1, 1, 1},   // 7
        {0, 0, 0, 0, 0, 0, 0},   // 8
        {0, 0, 0, 0, 1, 0, 0}    // 9
    };

    if (num > 9)
    {
        num = 0;
    }

    HAL_GPIO_WritePin(
        SEG0_GPIO_Port,
        SEG0_Pin,
        seg[num][0]
    );

    HAL_GPIO_WritePin(
        SEG1_GPIO_Port,
        SEG1_Pin,
        seg[num][1]
    );

    HAL_GPIO_WritePin(
        SEG2_GPIO_Port,
        SEG2_Pin,
        seg[num][2]
    );

    HAL_GPIO_WritePin(
        SEG3_GPIO_Port,
        SEG3_Pin,
        seg[num][3]
    );

    HAL_GPIO_WritePin(
        SEG4_GPIO_Port,
        SEG4_Pin,
        seg[num][4]
    );

    HAL_GPIO_WritePin(
        SEG5_GPIO_Port,
        SEG5_Pin,
        seg[num][5]
    );

    HAL_GPIO_WritePin(
        SEG6_GPIO_Port,
        SEG6_Pin,
        seg[num][6]
    );
}


/* =========================================================
   7-SEGMENT ENABLE

   PNP transistor:
   LOW  = ON
   HIGH = OFF
   ========================================================= */

void EN0_ON(void)
{
    HAL_GPIO_WritePin(
        EN0_GPIO_Port,
        EN0_Pin,
        GPIO_PIN_RESET
    );
}

void EN0_OFF(void)
{
    HAL_GPIO_WritePin(
        EN0_GPIO_Port,
        EN0_Pin,
        GPIO_PIN_SET
    );
}


void EN1_ON(void)
{
    HAL_GPIO_WritePin(
        EN1_GPIO_Port,
        EN1_Pin,
        GPIO_PIN_RESET
    );
}

void EN1_OFF(void)
{
    HAL_GPIO_WritePin(
        EN1_GPIO_Port,
        EN1_Pin,
        GPIO_PIN_SET
    );
}


void EN2_ON(void)
{
    HAL_GPIO_WritePin(
        EN2_GPIO_Port,
        EN2_Pin,
        GPIO_PIN_RESET
    );
}

void EN2_OFF(void)
{
    HAL_GPIO_WritePin(
        EN2_GPIO_Port,
        EN2_Pin,
        GPIO_PIN_SET
    );
}


void EN3_ON(void)
{
    HAL_GPIO_WritePin(
        EN3_GPIO_Port,
        EN3_Pin,
        GPIO_PIN_RESET
    );
}

void EN3_OFF(void)
{
    HAL_GPIO_WritePin(
        EN3_GPIO_Port,
        EN3_Pin,
        GPIO_PIN_SET
    );
}


void All_Display_Off(void)
{
    EN0_OFF();
    EN1_OFF();
    EN2_OFF();
    EN3_OFF();
}


/* =========================================================
   DIGITAL CLOCK
   ========================================================= */

const int MAX_LED = 4;

int index_led = 0;

int led_buffer[4] =
{
    0, 0, 0, 0
};


/*
 * Initial time
 *
 * 15:08:50
 */

int hour = 15;
int minute = 8;
int second = 50;


/* =========================================================
   UPDATE 7-SEGMENT
   ========================================================= */

void update7SEG(int index)
{
    /*
     * Turn OFF all displays first
     * to avoid ghosting.
     */

    EN0_OFF();
    EN1_OFF();
    EN2_OFF();
    EN3_OFF();


    /*
     * Select display
     */

    switch (index)
    {
        case 0:

            display7SEG(led_buffer[0]);
            EN0_ON();

            break;


        case 1:

            display7SEG(led_buffer[1]);
            EN1_ON();

            break;


        case 2:

            display7SEG(led_buffer[2]);
            EN2_ON();

            break;


        case 3:

            display7SEG(led_buffer[3]);
            EN3_ON();

            break;


        default:

            All_Display_Off();

            break;
    }
}


/* =========================================================
   UPDATE CLOCK BUFFER

   Display:

       HH:MM

   Example:

       15:08

   led_buffer = {1, 5, 0, 8}
   ========================================================= */

void updateClockBuffer(void)
{
    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;

    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}


/* =========================================================
   SOFTWARE TIMER 0

   Digital clock = 1 second
   ========================================================= */

volatile int timer0_counter = 0;
volatile int timer0_flag = 0;


/* =========================================================
   SOFTWARE TIMER 1

   7-segment scanning = 250 ms / digit
   ========================================================= */

volatile int timer1_counter = 0;
volatile int timer1_flag = 0;


/* =========================================================
   SOFTWARE TIMER MATRIX

   LED Matrix scanning = 10 ms / column
   ========================================================= */

volatile int timerMatrix_counter = 0;
volatile int timerMatrix_flag = 0;


/*
 * TIM2 interrupt period = 10 ms
 */

int TIMER_CYCLE = 10;


/* =========================================================
   TIMER 0
   ========================================================= */

void setTimer0(int duration)
{
    timer0_counter = duration / TIMER_CYCLE;
    timer0_flag = 0;
}


void timer_run(void)
{
    if (timer0_counter > 0)
    {
        timer0_counter--;

        if (timer0_counter == 0)
        {
            timer0_flag = 1;
        }
    }
}


/* =========================================================
   TIMER 1

   7-SEGMENT SCANNING
   ========================================================= */

void setTimer1(int duration)
{
    timer1_counter = duration / TIMER_CYCLE;
    timer1_flag = 0;
}


void timer1_run(void)
{
    if (timer1_counter > 0)
    {
        timer1_counter--;

        if (timer1_counter == 0)
        {
            timer1_flag = 1;
        }
    }
}


/* =========================================================
   MATRIX TIMER
   ========================================================= */

void setTimerMatrix(int duration)
{
    timerMatrix_counter = duration / TIMER_CYCLE;
    timerMatrix_flag = 0;
}


void timerMatrix_run(void)
{
    if (timerMatrix_counter > 0)
    {
        timerMatrix_counter--;

        if (timerMatrix_counter == 0)
        {
            timerMatrix_flag = 1;
        }
    }
}


/* =========================================================
   LED MATRIX 8x8

   Matrix columns:

   ENM0 -> PA2
   ENM1 -> PA3
   ENM2 -> PA10
   ENM3 -> PA11
   ENM4 -> PA12
   ENM5 -> PA13
   ENM6 -> PA14
   ENM7 -> PA15

   Matrix rows / data:

   ROW0 -> PB8
   ROW1 -> PB9
   ROW2 -> PB10
   ROW3 -> PB11
   ROW4 -> PB12
   ROW5 -> PB13
   ROW6 -> PB14
   ROW7 -> PB15
   ========================================================= */


/*
 * Number of matrix columns
 */

const int MAX_LED_MATRIX = 8;


/*
 * Current column
 */

int index_led_matrix = 0;


/*
 * Character A
 *
 *      00111100
 *      01100110
 *      11000011
 *      11000011
 *      11111111
 *      11000011
 *      11000011
 *      11000011
 *
 *
 * Each byte represents the data sent
 * to ROW0...ROW7.
 */

uint8_t matrix_buffer[8] =
{
    0xFC,
    0xFE,
    0x13,
    0x13,
    0x13,
    0x13,
    0xFE,
    0xFC
};

/* =========================================================
   MATRIX SCROLLING ANIMATION
   ========================================================= */

const uint8_t matrix_A[8] =
{
    0xFC,
    0xFE,
    0x13,
    0x11,
    0x11,
    0x13,
    0xFE,
    0xFC
};

int matrix_scroll_position = 0;


/* =========================================================
   TURN OFF ALL MATRIX COLUMNS
   ========================================================= */

void Matrix_All_Off(void)
{
    /*
     * ULN2803:
     *
     * ENM = HIGH
     * -> ULN output LOW
     * -> COL LOW
     * -> Column OFF
     */

    HAL_GPIO_WritePin(
        ENM0_GPIO_Port,
        ENM0_Pin,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ENM1_GPIO_Port,
        ENM1_Pin,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ENM2_GPIO_Port,
        ENM2_Pin,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ENM3_GPIO_Port,
        ENM3_Pin,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ENM4_GPIO_Port,
        ENM4_Pin,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ENM5_GPIO_Port,
        ENM5_Pin,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ENM6_GPIO_Port,
        ENM6_Pin,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ENM7_GPIO_Port,
        ENM7_Pin,
        GPIO_PIN_SET
    );
}

/* =========================================================
   SET MATRIX ROW DATA

   matrix_buffer[index]
   -> ROW0...ROW7
   ========================================================= */

void Matrix_Set_Row(uint8_t data)
{
    /*
     * MATRIX-8X8-RED:
     *
     * ROW = LOW  -> LED ON
     * ROW = HIGH -> LED OFF
     *
     * Therefore:
     *
     * bit = 1 -> LOW
     * bit = 0 -> HIGH
     */

    HAL_GPIO_WritePin(
        ROW0_GPIO_Port,
        ROW0_Pin,
        (data & 0x01)
            ? GPIO_PIN_RESET
            : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ROW1_GPIO_Port,
        ROW1_Pin,
        (data & 0x02)
            ? GPIO_PIN_RESET
            : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ROW2_GPIO_Port,
        ROW2_Pin,
        (data & 0x04)
            ? GPIO_PIN_RESET
            : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ROW3_GPIO_Port,
        ROW3_Pin,
        (data & 0x08)
            ? GPIO_PIN_RESET
            : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ROW4_GPIO_Port,
        ROW4_Pin,
        (data & 0x10)
            ? GPIO_PIN_RESET
            : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ROW5_GPIO_Port,
        ROW5_Pin,
        (data & 0x20)
            ? GPIO_PIN_RESET
            : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ROW6_GPIO_Port,
        ROW6_Pin,
        (data & 0x40)
            ? GPIO_PIN_RESET
            : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        ROW7_GPIO_Port,
        ROW7_Pin,
        (data & 0x80)
            ? GPIO_PIN_RESET
            : GPIO_PIN_SET
    );
}


/* =========================================================
   UPDATE LED MATRIX

   index = current column

   matrix_buffer[index]
   -> ROW0...ROW7

   Then enable ENM[index]
   ========================================================= */

void updateLEDMatrix(int index)
{
    if (index < 0 || index >= MAX_LED_MATRIX)
    {
        Matrix_All_Off();
        return;
    }

    /*
     * Turn OFF all columns first
     */
    Matrix_All_Off();

    /*
     * Put data on ROW0...ROW7
     */
    Matrix_Set_Row(matrix_buffer[index]);

    /*
     * Select ONE column.
     *
     * ENM = LOW
     * -> ULN2803 OFF
     * -> resistor pulls COL to +3.3V
     * -> column ON
     */

    switch (index)
    {
        case 0:

            HAL_GPIO_WritePin(
                ENM0_GPIO_Port,
                ENM0_Pin,
                GPIO_PIN_RESET
            );

            break;

        case 1:

            HAL_GPIO_WritePin(
                ENM1_GPIO_Port,
                ENM1_Pin,
                GPIO_PIN_RESET
            );

            break;

        case 2:

            HAL_GPIO_WritePin(
                ENM2_GPIO_Port,
                ENM2_Pin,
                GPIO_PIN_RESET
            );

            break;

        case 3:

            HAL_GPIO_WritePin(
                ENM3_GPIO_Port,
                ENM3_Pin,
                GPIO_PIN_RESET
            );

            break;

        case 4:

            HAL_GPIO_WritePin(
                ENM4_GPIO_Port,
                ENM4_Pin,
                GPIO_PIN_RESET
            );

            break;

        case 5:

            HAL_GPIO_WritePin(
                ENM5_GPIO_Port,
                ENM5_Pin,
                GPIO_PIN_RESET
            );

            break;

        case 6:

            HAL_GPIO_WritePin(
                ENM6_GPIO_Port,
                ENM6_Pin,
                GPIO_PIN_RESET
            );

            break;

        case 7:

            HAL_GPIO_WritePin(
                ENM7_GPIO_Port,
                ENM7_Pin,
                GPIO_PIN_RESET
            );

            break;

        default:

            Matrix_All_Off();

            break;
    }
}


void setTimerAnimation(int duration)
{
    timerAnimation_counter = duration / TIMER_CYCLE;
    timerAnimation_flag = 0;
}


void timerAnimation_run(void)
{
    if (timerAnimation_counter > 0)
    {
        timerAnimation_counter--;

        if (timerAnimation_counter == 0)
        {
            timerAnimation_flag = 1;
        }
    }
}

void updateMatrixAnimation(void)
{
    int i;
    int source_index;

    /*
     * Virtual screen:
     *
     * 8 columns blank
     * + 8 columns character A
     * + 8 columns blank
     *
     * Total = 24 columns
     */

    for (i = 0; i < 8; i++)
    {
        source_index = matrix_scroll_position + i;

        if (source_index >= 8 && source_index < 16)
        {
            /*
             * Character A is currently visible
             */

            matrix_buffer[i] =
                matrix_A[source_index - 8];
        }
        else
        {
            /*
             * Outside character A
             */

            matrix_buffer[i] = 0x00;
        }
    }


    /*
     * Move one column to the left
     */

    matrix_scroll_position++;


    /*
     * After A completely leaves the screen,
     * start again from the right side.
     */

    if (matrix_scroll_position > 16)
    {
        matrix_scroll_position = 0;
    }
}
/* USER CODE END 0 */


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/

    HAL_Init();

    SystemClock_Config();


    /*
     * Initialize peripherals
     */

    MX_GPIO_Init();
    MX_TIM2_Init();


    /* USER CODE BEGIN 2 */


    /*
     * Turn OFF all 7-segment displays
     */

    EN0_OFF();
    EN1_OFF();
    EN2_OFF();
    EN3_OFF();


    /*
     * Turn OFF LED Matrix
     */

    Matrix_All_Off();


    /*
     * Turn OFF DOT
     */

    HAL_GPIO_WritePin(
        DOT_GPIO_Port,
        DOT_Pin,
        GPIO_PIN_SET
    );


    /*
     * Turn OFF red LED initially
     */

    HAL_GPIO_WritePin(
        LED_RED_GPIO_Port,
        LED_RED_Pin,
        GPIO_PIN_RESET
    );


    /*
     * Initial clock display
     */

    updateClockBuffer();


    /*
     * Start TIM2 interrupt
     */

    HAL_TIM_Base_Start_IT(&htim2);


    /*
     * Timer 0:
     *
     * Digital clock = 1 second
     */

    setTimer0(1000);


    /*
     * Timer 1:
     *
     * 7SEG = 250 ms / digit
     */

    setTimer1(250);


    /*
     * Matrix:
     *
     * 10 ms / column
     */

    setTimerMatrix(10);
    setTimerAnimation(150);

    /* USER CODE END 2 */


    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1)
{
    /* 7 SEGMENT */

    if (timer1_flag == 1)
    {
        timer1_flag = 0;

        update7SEG(index_led);

        index_led++;

        if (index_led >= MAX_LED)
        {
            index_led = 0;
        }

        setTimer1(250);
    }


    /* LED MATRIX SCANNING */

    if (timerMatrix_flag == 1)
    {
        timerMatrix_flag = 0;

        updateLEDMatrix(index_led_matrix);

        index_led_matrix++;

        if (index_led_matrix >= MAX_LED_MATRIX)
        {
            index_led_matrix = 0;
        }

        setTimerMatrix(10);
    }


    /* MATRIX ANIMATION */

    if (timerAnimation_flag == 1)
    {
        timerAnimation_flag = 0;

        updateMatrixAnimation();

        setTimerAnimation(150);
    }


    /* DIGITAL CLOCK */

    if (timer0_flag == 1)
    {
        timer0_flag = 0;

        second++;

        if (second >= 60)
        {
            second = 0;
            minute++;
        }

        if (minute >= 60)
        {
            minute = 0;
            hour++;
        }

        if (hour >= 24)
        {
            hour = 0;
        }

        updateClockBuffer();

        HAL_GPIO_TogglePin(
            DOT_GPIO_Port,
            DOT_Pin
        );

        setTimer0(1000);
    }
}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* USER CODE END 3 */
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


    /*
     * HSI = 8 MHz
     */

    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;

    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;

    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_NONE;


    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }


    /*
     * CPU / AHB / APB clocks
     */

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_HSI;

    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV1;

    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;


    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}


/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};


    htim2.Instance = TIM2;


    /*
     * 8 MHz / (7999 + 1) = 1000 Hz
     *
     * Period = 9
     *
     * 1000 Hz / (9 + 1) = 100 Hz
     *
     * Interrupt = 10 ms
     */

    htim2.Init.Prescaler = 7999;

    htim2.Init.CounterMode =
        TIM_COUNTERMODE_UP;

    htim2.Init.Period = 9;

    htim2.Init.ClockDivision =
        TIM_CLOCKDIVISION_DIV1;

    htim2.Init.AutoReloadPreload =
        TIM_AUTORELOAD_PRELOAD_DISABLE;


    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }


    sClockSourceConfig.ClockSource =
        TIM_CLOCKSOURCE_INTERNAL;


    if (HAL_TIM_ConfigClockSource(
            &htim2,
            &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }


    sMasterConfig.MasterOutputTrigger =
        TIM_TRGO_RESET;

    sMasterConfig.MasterSlaveMode =
        TIM_MASTERSLAVEMODE_DISABLE;


    if (HAL_TIMEx_MasterConfigSynchronization(
            &htim2,
            &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    /* =====================================================
       GPIO CLOCK ENABLE
       ===================================================== */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();


    /*
     * PA13, PA14 and PA15 are normally used by
     * SWJ/JTAG/SWD.
     *
     * They are used here for ENM5, ENM6 and ENM7.
     */

    __HAL_RCC_AFIO_CLK_ENABLE();

    __HAL_AFIO_REMAP_SWJ_DISABLE();


    /* =====================================================
       GPIOA INITIAL OUTPUT LEVEL
       ===================================================== */


    /*
     * 7SEG enable:
     *
     * HIGH = OFF
     */

    HAL_GPIO_WritePin(
        GPIOA,
        EN0_Pin |
        EN1_Pin |
        EN2_Pin |
        EN3_Pin,
        GPIO_PIN_SET
    );


    /*
     * Matrix enable:
     *
     * LOW = OFF
     */

    HAL_GPIO_WritePin(
        GPIOA,
        ENM0_Pin |
        ENM1_Pin |
        ENM2_Pin |
        ENM3_Pin |
        ENM4_Pin |
        ENM5_Pin |
        ENM6_Pin |
        ENM7_Pin,
        GPIO_PIN_RESET
    );


    /*
     * DOT OFF
     *
     * HIGH = OFF
     */

    HAL_GPIO_WritePin(
        GPIOA,
        DOT_Pin,
        GPIO_PIN_SET
    );


    /*
     * RED LED OFF initially
     */

    HAL_GPIO_WritePin(
        GPIOA,
        LED_RED_Pin,
        GPIO_PIN_RESET
    );


    /* =====================================================
       GPIOB INITIAL OUTPUT LEVEL
       ===================================================== */


    /*
     * 7SEG segments OFF
     *
     * Common Anode:
     *
     * HIGH = OFF
     */

    HAL_GPIO_WritePin(
        GPIOB,
        SEG0_Pin |
        SEG1_Pin |
        SEG2_Pin |
        SEG3_Pin |
        SEG4_Pin |
        SEG5_Pin |
        SEG6_Pin,
        GPIO_PIN_SET
    );


    /*
     * Matrix rows OFF
     */

    HAL_GPIO_WritePin(
        GPIOB,
        ROW0_Pin |
        ROW1_Pin |
        ROW2_Pin |
        ROW3_Pin |
        ROW4_Pin |
        ROW5_Pin |
        ROW6_Pin |
        ROW7_Pin,
        GPIO_PIN_RESET
    );


    /* =====================================================
       GPIOA CONFIGURATION
       ===================================================== */

    GPIO_InitStruct.Pin =
        ENM0_Pin |
        ENM1_Pin |
        DOT_Pin |
        LED_RED_Pin |
        EN0_Pin |
        EN1_Pin |
        EN2_Pin |
        EN3_Pin |
        ENM2_Pin |
        ENM3_Pin |
        ENM4_Pin |
        ENM5_Pin |
        ENM6_Pin |
        ENM7_Pin;

    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pull =
        GPIO_NOPULL;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;


    HAL_GPIO_Init(
        GPIOA,
        &GPIO_InitStruct
    );


    /* =====================================================
       GPIOB CONFIGURATION
       ===================================================== */

    GPIO_InitStruct.Pin =
        SEG0_Pin |
        SEG1_Pin |
        SEG2_Pin |
        SEG3_Pin |
        SEG4_Pin |
        SEG5_Pin |
        SEG6_Pin |
        ROW0_Pin |
        ROW1_Pin |
        ROW2_Pin |
        ROW3_Pin |
        ROW4_Pin |
        ROW5_Pin |
        ROW6_Pin |
        ROW7_Pin;

    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pull =
        GPIO_NOPULL;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;


    HAL_GPIO_Init(
        GPIOB,
        &GPIO_InitStruct
    );
}


/* =========================================================
   TIM2 CALLBACK

   Exercise 8/9:

   Interrupt ONLY handles software timers.

   No display processing is performed here.
   ========================================================= */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        timer_run();
        timer1_run();
        timerMatrix_run();
        timerAnimation_run();
    }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /*
     * User can add implementation here.
     */
}

#endif /* USE_FULL_ASSERT */
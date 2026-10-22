#include "input_reading.h"

#define BUTTON_COUNT        3U
#define DEBOUNCE_SAMPLES    3U

typedef struct {
    uint8_t candidate;
    uint8_t stable;
    uint8_t count;
    volatile uint8_t pending_press;
} ButtonData;

static ButtonData buttons[BUTTON_COUNT];

/* Read raw button state: pressed = 1 */
static uint8_t button_raw(uint8_t index)
{
    GPIO_PinState pin;

    switch (index) {
        case 0:
            pin = HAL_GPIO_ReadPin(BT1_GPIO_Port, BT1_Pin);
            break;

        case 1:
            pin = HAL_GPIO_ReadPin(BT2_GPIO_Port, BT2_Pin);
            break;

        case 2:
            pin = HAL_GPIO_ReadPin(BT3_GPIO_Port, BT3_Pin);
            break;

        default:
            return 0U;
    }

    return (pin == GPIO_PIN_RESET) ? 1U : 0U;
}

void button_init(void)
{
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        uint8_t raw = button_raw(i);

        buttons[i].candidate = raw;
        buttons[i].stable = raw;
        buttons[i].count = DEBOUNCE_SAMPLES;
        buttons[i].pending_press = 0U;
    }
}

/* Called every 10 ms */
void button_reading(void)
{
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        ButtonData *b = &buttons[i];
        uint8_t raw = button_raw(i);

        if (raw != b->candidate) {
            b->candidate = raw;
            b->count = 1U;
        }
        else if (b->count < DEBOUNCE_SAMPLES) {
            b->count++;
        }

        if (b->count >= DEBOUNCE_SAMPLES &&
            b->stable != b->candidate) {

            b->stable = b->candidate;

            /* Generate event only on press */
            if (b->stable == 1U) {
                b->pending_press = 1U;
            }
        }
    }
}

/* Consume one button press event */
uint8_t button_take_press(uint8_t index)
{
    if (index >= BUTTON_COUNT) {
        return 0U;
    }

    uint32_t primask = __get_PRIMASK();

    __disable_irq();

    uint8_t event = buttons[index].pending_press;
    buttons[index].pending_press = 0U;

    __set_PRIMASK(primask);

    return event;
}
#include "main.h"
#include "input_reading.h"

#define N0_OF_BUTTONS       3

#define BUTTON_IS_PRESSED  GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

static GPIO_PinState buttonBuffer[N0_OF_BUTTONS];

static GPIO_PinState debounceButtonBuffer1[N0_OF_BUTTONS] =
{
    BUTTON_IS_RELEASED,
    BUTTON_IS_RELEASED,
    BUTTON_IS_RELEASED
};

static GPIO_PinState debounceButtonBuffer2[N0_OF_BUTTONS] =
{
    BUTTON_IS_RELEASED,
    BUTTON_IS_RELEASED,
    BUTTON_IS_RELEASED
};

void button_reading(void)
{
    GPIO_PinState current[N0_OF_BUTTONS];

    current[0] = HAL_GPIO_ReadPin(BT1_GPIO_Port, BT1_Pin);
    current[1] = HAL_GPIO_ReadPin(BT2_GPIO_Port, BT2_Pin);
    current[2] = HAL_GPIO_ReadPin(BT3_GPIO_Port, BT3_Pin);

    for (int i = 0; i < N0_OF_BUTTONS; i++)
    {
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = current[i];

        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i])
        {
            buttonBuffer[i] = debounceButtonBuffer1[i];
        }
    }
}

GPIO_PinState is_button_pressed(uint8_t index)
{
    if (index >= N0_OF_BUTTONS)
    {
        return BUTTON_IS_RELEASED;
    }

    return buttonBuffer[index];
}

void restoreBuffer(uint8_t index)
{
    if (index < N0_OF_BUTTONS)
    {
        buttonBuffer[index] = BUTTON_IS_RELEASED;
    }
}
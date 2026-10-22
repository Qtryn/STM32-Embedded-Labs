#include "main.h"
#include "input_reading.h"
#include "input_processing.h"

#define BUTTON_IS_PRESSED  GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

typedef enum
{
    MODE_1_NORMAL = 1,
    MODE_2_RED_MODIFY,
    MODE_3_AMBER_MODIFY,
    MODE_4_GREEN_MODIFY
} Mode;

typedef enum
{
    RELEASE,
    PRESS
} ButtonState;

static Mode currentMode = MODE_1_NORMAL;

static ButtonState previousState[3] =
{
    RELEASE,
    RELEASE,
    RELEASE
};

static ButtonState currentState[3] =
{
    RELEASE,
    RELEASE,
    RELEASE
};

void fsm_for_input_processing(void)
{
    for (int i = 0; i < 3; i++)
    {
        previousState[i] = currentState[i];

        if (is_button_pressed(i) == BUTTON_IS_PRESSED)
        {
            currentState[i] = PRESS;
        }
        else
        {
            currentState[i] = RELEASE;
        }
    }

    /* Button 1: change mode */
    if (currentState[0] == PRESS &&
        previousState[0] == RELEASE)
    {
        currentMode++;

        if (currentMode > MODE_4_GREEN_MODIFY)
        {
            currentMode = MODE_1_NORMAL;
        }
        HAL_GPIO_TogglePin(RED_NS_GPIO_Port, RED_NS_Pin);
    }
}

uint8_t getCurrentMode(void)
{
    return (uint8_t)currentMode;
}
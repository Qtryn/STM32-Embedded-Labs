#include "input_processing.h"
#include "input_reading.h"

/* Current mode */
static uint8_t mode = MODE_1_NORMAL;

/* Saved time values, in seconds */
static uint8_t red_saved = 5U;
static uint8_t amber_saved = 2U;
static uint8_t green_saved = 3U;

/* Temporary value being edited */
static uint8_t edit_value = 5U;

/* Incremented after every successful BT3 save */
static uint8_t save_sequence = 0U;

/* Return saved time for selected mode */
uint8_t getSavedTimeForMode(uint8_t requested_mode)
{
    switch (requested_mode) {
        case MODE_2_RED:
            return red_saved;

        case MODE_3_YELLOW:
            return amber_saved;

        case MODE_4_GREEN:
            return green_saved;

        default:
            return 0U;
    }
}

/* Main button FSM */
void fsm_for_input_processing(void)
{
    uint8_t b1 = button_take_press(0);
    uint8_t b2 = button_take_press(1);
    uint8_t b3 = button_take_press(2);

    /* BT1: Mode 1 -> 2 -> 3 -> 4 -> 1 */
    if (b1) {
        mode = (mode == MODE_4_GREEN)
             ? MODE_1_NORMAL
             : (uint8_t)(mode + 1U);

        /* Restore previously saved value */
        if (mode != MODE_1_NORMAL) {
            edit_value = getSavedTimeForMode(mode);
        }

        return;
    }

    /* BT2 and BT3 do nothing in Normal Mode */
    if (mode == MODE_1_NORMAL) {
        return;
    }

    /* BT2: Increase time, range 1..99 */
    if (b2) {
        edit_value = (edit_value == 99U)
                   ? 1U
                   : (uint8_t)(edit_value + 1U);
    }

    /* BT3: Save selected time */
    if (b3) {
        switch (mode) {
            case MODE_2_RED:
                red_saved = edit_value;
                break;

            case MODE_3_YELLOW:
                amber_saved = edit_value;
                break;

            case MODE_4_GREEN:
                green_saved = edit_value;
                break;

            default:
                break;
        }

        save_sequence++;
    }
}

uint8_t getCurrentMode(void)
{
    return mode;
}

uint8_t getRedTime(void)
{
    return red_saved;
}

uint8_t getAmberTime(void)
{
    return amber_saved;
}

uint8_t getGreenTime(void)
{
    return green_saved;
}

uint8_t getCurrentEditValue(void)
{
    return edit_value;
}

uint8_t getSaveSequence(void)
{
    return save_sequence;
}
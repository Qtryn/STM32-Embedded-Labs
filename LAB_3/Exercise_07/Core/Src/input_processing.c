#include "input_processing.h"
#include "input_reading.h"

static uint8_t mode = MODE_1_NORMAL;

/* Last fully VALID configuration, active in Mode 1. */
static uint8_t red_saved = 5U;
static uint8_t amber_saved = 2U;
static uint8_t green_saved = 3U;

/* Draft configuration: edited across Modes 2, 3, and 4. */
static uint8_t red_draft = 5U;
static uint8_t amber_draft = 2U;
static uint8_t green_draft = 3U;
static uint8_t edit_value = 5U;

static uint8_t save_sequence = 0U;
static uint8_t invalid_sequence = 0U;

static void restore_drafts(void)
{
    red_draft = red_saved;
    amber_draft = amber_saved;
    green_draft = green_saved;
}

static uint8_t drafts_are_valid(void)
{
    uint16_t sum = (uint16_t)green_draft + (uint16_t)amber_draft;

    /* Each draft is already within [1, 99] by construction. */
    return (sum <= 99U && (uint16_t)red_draft == sum) ? 1U : 0U;
}

uint8_t getSavedTimeForMode(uint8_t requested_mode)
{
    /* This getter intentionally returns the STAGED value for the UI. */
    switch (requested_mode) {
        case MODE_2_RED:    return red_draft;
        case MODE_3_YELLOW: return amber_draft;
        case MODE_4_GREEN:  return green_draft;
        default:            return 0U;
    }
}

void fsm_for_input_processing(void)
{
    uint8_t b1 = button_take_press(0U);
    uint8_t b2 = button_take_press(1U);
    uint8_t b3 = button_take_press(2U);

    if (b1 != 0U) {
        if (mode == MODE_4_GREEN) {
            /* Atomic validation of the entire R/Y/G draft. */
            if (drafts_are_valid() != 0U) {
                red_saved = red_draft;
                amber_saved = amber_draft;
                green_saved = green_draft;
            } else {
                /* Reject all draft edits; retain the old valid settings. */
                restore_drafts();
                invalid_sequence++;
            }
            mode = MODE_1_NORMAL;
        } else {
            if (mode == MODE_1_NORMAL) {
                /* Start a fresh edit transaction. */
                restore_drafts();
            }
            mode = (uint8_t)(mode + 1U);
            edit_value = getSavedTimeForMode(mode);
        }
        return;
    }

    if (mode == MODE_1_NORMAL) {
        return;
    }

    if (b2 != 0U) {
        edit_value = (edit_value == 99U) ? 1U : (uint8_t)(edit_value + 1U);
    }

    if (b3 != 0U) {
        /* BT3 stages one color; validation/commit happens at Mode 4 -> 1. */
        switch (mode) {
            case MODE_2_RED:    red_draft = edit_value; break;
            case MODE_3_YELLOW: amber_draft = edit_value; break;
            case MODE_4_GREEN:  green_draft = edit_value; break;
            default: break;
        }
        save_sequence++;
    }
}

uint8_t getCurrentMode(void) { return mode; }
uint8_t getRedTime(void) { return red_saved; }
uint8_t getAmberTime(void) { return amber_saved; }
uint8_t getGreenTime(void) { return green_saved; }
uint8_t getCurrentEditValue(void) { return edit_value; }
uint8_t getSaveSequence(void) { return save_sequence; }
uint8_t getInvalidSequence(void) { return invalid_sequence; }

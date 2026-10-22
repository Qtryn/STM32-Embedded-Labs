#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

#include <stdint.h>

#define MODE_1_NORMAL  1U
#define MODE_2_RED     2U
#define MODE_3_YELLOW  3U
#define MODE_4_GREEN   4U

void fsm_for_input_processing(void);
uint8_t getCurrentMode(void);

/* Validated values used by normal traffic mode. */
uint8_t getRedTime(void);
uint8_t getAmberTime(void);
uint8_t getGreenTime(void);

/* Draft values used by modification modes. */
uint8_t getCurrentEditValue(void);
uint8_t getSavedTimeForMode(uint8_t mode);

/* Event counters; incremented when BT3 stages a value or validation fails. */
uint8_t getSaveSequence(void);
uint8_t getInvalidSequence(void);

#endif

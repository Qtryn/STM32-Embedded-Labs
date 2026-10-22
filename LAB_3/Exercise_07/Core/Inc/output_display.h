#ifndef INC_OUTPUT_DISPLAY_H_
#define INC_OUTPUT_DISPLAY_H_

#include "main.h"

void display_init(void);
void display_show_values(uint8_t left, uint8_t right);
void display_show_error(void);  /* EEEE on common-anode seven-segment */
void display_scan_next(void);

#endif

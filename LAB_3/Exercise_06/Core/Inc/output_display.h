#ifndef INC_OUTPUT_DISPLAY_H_
#define INC_OUTPUT_DISPLAY_H_

#include "main.h"

void display7SEG(uint8_t num);
void turnOffAll7SEG(void);

void EN0_ON(void);
void EN0_OFF(void);

void EN1_ON(void);
void EN1_OFF(void);

void EN2_ON(void);
void EN2_OFF(void);

void EN3_ON(void);
void EN3_OFF(void);

void All_Display_Off(void);

void updateMode7SEG(uint8_t mode, uint8_t index);
#endif /* INC_OUTPUT_DISPLAY_H_ */
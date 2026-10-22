#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

#include "main.h"

void button_reading(void);
GPIO_PinState is_button_pressed(uint8_t index);
void restoreBuffer(uint8_t index);

#endif /* INC_INPUT_READING_H_ */
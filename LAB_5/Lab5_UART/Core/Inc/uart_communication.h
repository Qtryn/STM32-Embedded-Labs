#ifndef UART_COMMUNICATION_H
#define UART_COMMUNICATION_H

#include "stm32f1xx_hal.h"
#include "command_parser.h"

void uart_communication_init(ADC_HandleTypeDef *adc, UART_HandleTypeDef *uart);
void uart_communication_process(command_t command);
void uart_communication_fsm(void);

#endif

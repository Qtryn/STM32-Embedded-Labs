#ifndef TEST_STM32F1XX_HAL_H
#define TEST_STM32F1XX_HAL_H

#include <stdint.h>

typedef struct { int unused; } ADC_HandleTypeDef;
typedef struct { int unused; } UART_HandleTypeDef;
typedef enum { HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT } HAL_StatusTypeDef;

uint32_t HAL_GetTick(void);
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *adc, uint32_t timeout);
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *adc);
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *uart, const uint8_t *data,
                                    uint16_t length, uint32_t timeout);

#endif

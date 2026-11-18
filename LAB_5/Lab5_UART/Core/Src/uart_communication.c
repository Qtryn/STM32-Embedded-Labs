#include "uart_communication.h"
#include <stdio.h>

typedef enum { COMM_IDLE, COMM_WAIT_ACK } communication_state_t;

static communication_state_t state;
static ADC_HandleTypeDef *adc_handle;
static UART_HandleTypeDef *uart_handle;
static char packet[16];
static uint16_t packet_length;
static uint32_t last_send_tick;

static void send_packet(void)
{
    HAL_UART_Transmit(uart_handle, (const uint8_t *)packet, packet_length, 100);
    last_send_tick = HAL_GetTick();
}

void uart_communication_init(ADC_HandleTypeDef *adc, UART_HandleTypeDef *uart)
{
    adc_handle = adc;
    uart_handle = uart;
    state = COMM_IDLE;
    packet_length = 0;
    last_send_tick = 0;
}

void uart_communication_process(command_t command)
{
    if (state == COMM_WAIT_ACK)
    {
        if (command == COMMAND_OK)
        {
            state = COMM_IDLE;
        }
    }
    else if (command == COMMAND_RST)
    {
        if (HAL_ADC_PollForConversion(adc_handle, 10) != HAL_OK)
        {
            return;
        }

        uint32_t value = HAL_ADC_GetValue(adc_handle);
        int length = snprintf(packet, sizeof(packet), "!ADC=%lu#", (unsigned long)value);

        if (length > 0 && (unsigned int)length < sizeof(packet))
        {
            packet_length = (uint16_t)length;
            send_packet();
            state = COMM_WAIT_ACK;
        }
    }
}

void uart_communication_fsm(void)
{
    if (state == COMM_WAIT_ACK &&
        (uint32_t)(HAL_GetTick() - last_send_tick) >= 3000U)
    {
        send_packet();
    }
}

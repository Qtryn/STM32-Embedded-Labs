#include "command_parser.h"
#include "uart_communication.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static ADC_HandleTypeDef adc;
static UART_HandleTypeDef uart;
static uint32_t tick;
static uint32_t adc_value;
static unsigned int adc_reads;
static unsigned int polls;
static unsigned int sends;
static uint32_t transmit_time;
static HAL_StatusTypeDef adc_status;
static HAL_StatusTypeDef transmit_status;
static char packets[512][16];

uint32_t HAL_GetTick(void) { return tick; }

HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *handle, uint32_t timeout)
{
    assert(handle == &adc && timeout == 10);
    polls++;
    return adc_status;
}

uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *handle)
{
    assert(handle == &adc);
    adc_reads++;
    return adc_value;
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *handle, const uint8_t *data,
                                    uint16_t length, uint32_t timeout)
{
    assert(handle == &uart && timeout == 100);
    assert(sends < 512 && length < sizeof(packets[0]));
    assert(data[0] == '!' && data[length - 1] == '#');
    for (uint16_t i = 0; i < length; i++)
    {
        assert(data[i] != '\r' && data[i] != '\n' && data[i] != ' ' && data[i] != '\0');
    }
    memcpy(packets[sends], data, length);
    packets[sends++][length] = '\0';
    tick += transmit_time;
    return transmit_status;
}

static void reset(void)
{
    tick = 0;
    adc_value = 2048;
    adc_reads = polls = sends = 0;
    transmit_time = 0;
    adc_status = transmit_status = HAL_OK;
    memset(packets, 0, sizeof(packets));
    command_parser_init();
    uart_communication_init(&adc, &uart);
}

static void input(const char *text)
{
    while (*text)
    {
        uart_communication_process(command_parser_fsm((uint8_t)*text++));
    }
    uart_communication_fsm();
}

static void request_and_ack(void)
{
    reset();
    input("!RST#");
    assert(sends == 1 && adc_reads == 1 && polls == 1);
    assert(strcmp(packets[0], "!ADC=2048#") == 0);
    tick = 2999;
    input("!OK#");
    tick = 10000;
    uart_communication_fsm();
    assert(sends == 1);
}

static void saved_packet_and_new_sample(void)
{
    reset();
    input("!RST#");
    adc_value = 3500;
    tick = 2999;
    uart_communication_fsm();
    assert(sends == 1);
    tick = 3000;
    uart_communication_fsm();
    assert(sends == 2 && adc_reads == 1 && polls == 1);
    assert(strcmp(packets[0], packets[1]) == 0);
    tick = 5999;
    uart_communication_fsm();
    assert(sends == 2);
    tick = 6000;
    uart_communication_fsm();
    assert(sends == 3 && strcmp(packets[0], packets[2]) == 0);
    input("!OK#");
    tick = 12000;
    uart_communication_fsm();
    assert(sends == 3);
    input("!RST#");
    assert(sends == 4 && adc_reads == 2);
    assert(strcmp(packets[3], "!ADC=3500#") == 0);
}

static void malformed_and_incomplete(void)
{
    reset();
    input("!OK#!ABC#!RTS#!rst#RST#!#!!#\r\n");
    assert(sends == 0 && adc_reads == 0);
    input("!RST");
    assert(sends == 0);
    input("#");
    assert(sends == 1);
    input("!RST#!ABC#");
    assert(sends == 1 && adc_reads == 1);
    tick = 3000;
    uart_communication_fsm();
    assert(sends == 2);
}

static void oversized_and_restart(void)
{
    reset();
    input("!");
    for (unsigned int i = 0; i < 10000; i++)
    {
        assert(command_parser_fsm('X') == COMMAND_NONE);
    }
    input("RST#");
    assert(sends == 0);
    input("!invalid!RST#");
    assert(sends == 1);
    input("!O!OK#");
    tick = 4000;
    uart_communication_fsm();
    assert(sends == 1);
}

static void consecutive_commands(void)
{
    reset();
    input("!RST#!OK#!RST#!OK#!OK#!RST#");
    assert(sends == 3 && adc_reads == 3);
    tick = 3000;
    uart_communication_fsm();
    assert(sends == 4 && adc_reads == 3);
    input("!OK#");
    tick = 7000;
    uart_communication_fsm();
    assert(sends == 4);
}

static void adc_endpoints_and_failure(void)
{
    reset();
    adc_value = 0;
    input("!RST#!OK#");
    assert(strcmp(packets[0], "!ADC=0#") == 0);
    adc_value = 4095;
    input("!RST#!OK#");
    assert(strcmp(packets[1], "!ADC=4095#") == 0);
    adc_status = HAL_TIMEOUT;
    input("!RST#");
    assert(sends == 2 && adc_reads == 2);
    tick = 5000;
    uart_communication_fsm();
    assert(sends == 2);
    adc_status = HAL_OK;
    input("!RST#");
    assert(sends == 3 && adc_reads == 3);
}

static void timer_after_transmit_and_ack_at_deadline(void)
{
    reset();
    transmit_time = 12;
    input("!RST#");
    assert(tick == 12);
    tick = 3011;
    uart_communication_fsm();
    assert(sends == 1);
    tick = 3012;
    uart_communication_fsm();
    assert(sends == 2 && tick == 3024);
    tick = 6024;
    input("!OK#");
    assert(sends == 2);
}

static void tick_rollover(void)
{
    reset();
    tick = UINT32_MAX - 999U;
    input("!RST#");
    tick += 2999U;
    uart_communication_fsm();
    assert(sends == 1);
    tick += 1U;
    uart_communication_fsm();
    assert(sends == 2 && adc_reads == 1);
    input("!OK#");
    tick += 3000U;
    uart_communication_fsm();
    assert(sends == 2);
}

static void transmit_failure_retries_same_packet(void)
{
    reset();
    transmit_status = HAL_TIMEOUT;
    input("!RST#");
    assert(sends == 1 && adc_reads == 1);
    transmit_status = HAL_OK;
    adc_value = 123;
    tick = 3000;
    uart_communication_fsm();
    assert(sends == 2 && adc_reads == 1);
    assert(strcmp(packets[0], packets[1]) == 0);
}

static void ten_minutes_simulated_time(void)
{
    reset();
    input("!RST#");
    for (tick = 1; tick <= 600000; tick++)
    {
        adc_value = tick % 4096;
        uart_communication_fsm();
    }
    assert(sends == 201 && adc_reads == 1);
    for (unsigned int i = 1; i < sends; i++)
    {
        assert(strcmp(packets[0], packets[i]) == 0);
    }
    input("!OK#");
    tick += 30000;
    uart_communication_fsm();
    assert(sends == 201);
}

#define RUN(test) do { test(); puts("PASS " #test); } while (0)

int main(void)
{
    RUN(request_and_ack);
    RUN(saved_packet_and_new_sample);
    RUN(malformed_and_incomplete);
    RUN(oversized_and_restart);
    RUN(consecutive_commands);
    RUN(adc_endpoints_and_failure);
    RUN(timer_after_transmit_and_ack_at_deadline);
    RUN(tick_rollover);
    RUN(transmit_failure_retries_same_packet);
    RUN(ten_minutes_simulated_time);
    puts("All 10 host test groups passed (mock HAL, not Proteus).");
    return 0;
}

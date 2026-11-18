#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdint.h>

#define MAX_BUFFER_SIZE 30U

typedef enum
{
    COMMAND_NONE,
    COMMAND_RST,
    COMMAND_OK
} command_t;

void command_parser_init(void);
command_t command_parser_fsm(uint8_t byte);

#endif

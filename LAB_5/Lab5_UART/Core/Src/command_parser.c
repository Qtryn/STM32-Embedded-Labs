#include "command_parser.h"

typedef enum { WAIT_START, COLLECT } parser_state_t;

static parser_state_t state;
static uint8_t command[3];
static uint8_t length;

void command_parser_init(void)
{
    state = WAIT_START;
    length = 0;
}

command_t command_parser_fsm(uint8_t byte)
{
    command_t event = COMMAND_NONE;

    if (byte == '!')
    {
        state = COLLECT;
        length = 0;
    }
    else if (state == COLLECT)
    {
        if (byte == '#')
        {
            if (length == 3 && command[0] == 'R' &&
                command[1] == 'S' && command[2] == 'T')
            {
                event = COMMAND_RST;
            }
            else if (length == 2 && command[0] == 'O' && command[1] == 'K')
            {
                event = COMMAND_OK;
            }
            command_parser_init();
        }
        else if (length < sizeof(command))
        {
            command[length++] = byte;
        }
        else
        {
            command_parser_init();
        }
    }

    return event;
}

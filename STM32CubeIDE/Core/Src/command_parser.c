#include "command_parser.h"





#define MAX_BUFFER_SIZE 30
extern uint8_t buffer[MAX_BUFFER_SIZE];
extern volatile uint8_t index_buffer;

volatile uint8_t command_flag = 0;
volatile uint8_t command_data = CMD_NONE;

enum ParserState { P_IDLE, P_START, P_R, P_RS, P_RST, P_O, P_OK };
static enum ParserState p_state = P_IDLE;
static uint8_t index_read = 0;





static void parse_byte(uint8_t c)
{
    if (c == '!')
    	p_state = P_START; return;

    switch (p_state)
    {
		case P_START:
			if (c == 'R')      p_state = P_R;
			else if (c == 'O') p_state = P_O;
			else               p_state = P_IDLE;
			break;


		case P_R:
			p_state = (c == 'S') ? P_RS  : P_IDLE;
			break;


		case P_RS:
			p_state = (c == 'T') ? P_RST : P_IDLE;
			break;


		case P_RST:
			if (c == '#')
				command_data = CMD_RST; command_flag = 1;
			p_state = P_IDLE;
			break;


		case P_O:
			p_state = (c == 'K') ? P_OK : P_IDLE;
			break;


		case P_OK:
			if (c == '#')
				command_data = CMD_OK; command_flag = 1;
			p_state = P_IDLE;
			break;


		default:
			p_state = P_IDLE;
			break;
    }
}


/* Consumes EVERY byte received since the last call */
void command_parser_fsm(void)
{
    while (index_read != index_buffer)
    {
        uint8_t c = buffer[index_read];
        index_read++;
        if (index_read == MAX_BUFFER_SIZE)
        	index_read = 0;
        parse_byte(c);
    }
}

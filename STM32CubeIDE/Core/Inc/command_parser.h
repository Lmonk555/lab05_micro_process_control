#ifndef INC_COMMAND_PARSER_H_
#define INC_COMMAND_PARSER_H_





#include "main.h"

#define CMD_NONE 0
#define CMD_RST  1
#define CMD_OK   2

extern volatile uint8_t command_flag;
extern volatile uint8_t command_data;

void command_parser_fsm(void);





#endif

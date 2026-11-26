#include "uart_communication.h"
#include "command_parser.h"
#include <stdio.h>





extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

#define TIMEOUT_MS 3000

enum CommState { COMM_IDLE, COMM_WAIT_OK };
static enum CommState cstate = COMM_IDLE;

static uint32_t ADC_value = 0;
static uint32_t sent_tick = 0;
static char str[30];





static void send_packet(void)
{
    int len = sprintf(str, "!ADC=%lu#\r\n", (unsigned long)ADC_value);
    HAL_UART_Transmit(&huart2, (uint8_t *)str, len, 1000);
    sent_tick = HAL_GetTick();
}


void uart_communication_fsm(void)
{
    switch (cstate)
    {
    case COMM_IDLE:
        if (command_flag == 1)
        {
            command_flag = 0;

            if (command_data == CMD_RST)
            {
                ADC_value = HAL_ADC_GetValue(&hadc1);	// read a NEW value
                send_packet();
                cstate = COMM_WAIT_OK;
            }
        }
        break;


    case COMM_WAIT_OK:
        if (command_flag == 1)
        {
            command_flag = 0;
            if (command_data == CMD_OK)
            {
                cstate = COMM_IDLE;						// done, ready for next !RST#
                break;
            }
        }
        if (HAL_GetTick() - sent_tick >= TIMEOUT_MS)
        {
            send_packet();								// resend, ADC_value unchanged
        }
        break;


    default:
        cstate = COMM_IDLE;
        break;
    }
}

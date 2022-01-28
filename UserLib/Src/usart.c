//
// Created by Owen on 2022/1/24 14:29.
//

#include "usart.h"
#include "stdio.h"

void UART1_Transmit(uint8_t *Data)
{
    HAL_UART_Transmit(&huart1, Data, 1, 0xffff);
}

uint8_t UART1_Receive(uint8_t *pData)
{
    return HAL_UART_Receive(&huart1, pData, 1, 0xffff);
}

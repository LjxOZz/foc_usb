#ifndef __BSP_USART_H__
#define __BSP_USART_H__

//打印用的串口

#include "usart.h"
#include "main.h"

#include <stdio.h>

extern UART_HandleTypeDef huart1;




void Usart_Sendbyte(uint8_t byte);
void Usart_SendString(uint8_t * str);
void Usart_SendString_IT(uint8_t * str);
void Usart_SendArray(uint8_t* array_send, uint8_t size);



#endif


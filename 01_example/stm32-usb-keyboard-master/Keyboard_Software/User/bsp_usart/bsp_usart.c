#include "bsp_usart.h"



void Usart_SendString(uint8_t * str)
{
	uint8_t k=0;
	do
	{
		HAL_UART_Transmit(&huart1, str+k,1,1000);
		k++;
	}while(*(str+k)!='\0');
}

void Usart_Sendbyte(uint8_t byte)
{
		HAL_UART_Transmit(&huart1, &byte,1,1000);
}

void Usart_SendString_IT(uint8_t * str)
{
	uint8_t k=0;
	do
	{
		HAL_UART_Transmit_IT(&huart1, str+k,3);
		k++;
	}while(*(str+k)!='\0');
}

//��������
void Usart_SendArray(uint8_t* array_send, uint8_t size)
{
	
	HAL_UART_Transmit(&huart1, array_send, size, 1000);	//��������
	while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) != SET);		//�ȴ����ͽ���
}


///�ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����DEBUG_USART */
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}


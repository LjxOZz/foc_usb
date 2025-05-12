#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "gpio.h"
#include "main.h"

//�������ŵ�ѡ��
#define KEY1_GPIO_PORT GPIOA
#define KEY1_GPIO_PIN  GPIO_PIN_0

#define KEY2_GPIO_PORT GPIOC
#define KEY2_GPIO_PIN  GPIO_PIN_13
//��ת����������
#define Coder_DI_GPIO_PORT   GPIOC
#define Coder_DI_GPIO_PIN  GPIO_PIN_8
#define Coder_CLK_GPIO_PORT  GPIOC
#define Coder_CLK_GPIO_PIN GPIO_PIN_9

//��ȡ����״̬
#define KEY1_Input()    HAL_GPIO_ReadPin(KEY1_GPIO_PORT,KEY1_GPIO_PIN)
#define KEY2_Input()    HAL_GPIO_ReadPin(KEY2_GPIO_PORT,KEY2_GPIO_PIN)

//����������
#define Coder_DI_Input()    HAL_GPIO_ReadPin(Coder_DI_GPIO_PORT,Coder_DI_GPIO_PIN)
#define Coder_CLK_Input()   HAL_GPIO_ReadPin(Coder_CLK_GPIO_PORT,Coder_CLK_GPIO_PIN)

#define KEY_ON     GPIO_PIN_SET
#define KEY_OFF    GPIO_PIN_RESET

//�������͵�ƽ��Ч
#define Coder_ON      GPIO_PIN_RESET
#define Coder_OFF     GPIO_PIN_SET

//������״̬
#define Static_State  0
#define Ready_Clock  1
#define Ready_AntiClock  2
#define Down_Clock 3
#define Down_AntiClock 4




#endif

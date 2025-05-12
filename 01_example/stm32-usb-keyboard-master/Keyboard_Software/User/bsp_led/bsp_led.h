#ifndef __BSP_LED_H__
#define __BSP_LED_H__


#include "gpio.h"
#include "main.h"

//µÆµÄºê¶¨Òå
#define  LED0_GPIO_PIN      GPIO_PIN_0
#define  LED0_GPIO_PORT      GPIOB
#define  LED1_GPIO_PIN      GPIO_PIN_1
#define  LED1_GPIO_PORT      GPIOB

#define  KEY0_GPIO_PIN      GPIO_PIN_4
#define  KEY0_GPIO_PORT      GPIOE


#define LED0_ON()    HAL_GPIO_WritePin(LED0_GPIO_PORT,LED0_GPIO_PIN,GPIO_PIN_RESET)
#define LED1_ON()    HAL_GPIO_WritePin(LED1_GPIO_PORT,LED1_GPIO_PIN,GPIO_PIN_RESET)
#define LED0_OFF()    HAL_GPIO_WritePin(LED0_GPIO_PORT,LED0_GPIO_PIN,GPIO_PIN_SET)
#define LED1_OFF()    HAL_GPIO_WritePin(LED1_GPIO_PORT,LED1_GPIO_PIN,GPIO_PIN_SET)

#define LED1_Toggle()    HAL_GPIO_TogglePin(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED0_Toggle()    HAL_GPIO_TogglePin(LED0_GPIO_PORT,LED0_GPIO_PIN)

#define KEY0_read    HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN)  
#endif


#ifndef _BSP_DRV_H_
#define _BSP_DRV_H_

#include "main.h"



//ADD:0x02
#define GATE_CURRENT 		(0x01 << 0)		
#define GATE_RESET			(0x00 << 2)
#define PWM_MODE			(0x00 << 3)
#define OCP_MODE			(0x01 << 4)

//ADD:0x03
//#define OCTW_MODE
//#define GAINVALUE 
//#define DC_CAL_CH1
//#define DC_CAL_CH2 
//#define OC_TOFF
//WriteConfig = DRV8301WRITE | DRV8301ADD3 | OCTW_MODE | GAINVALUE | DC_CAL_CH1 | DC_CAL_CH2 | OC_TOFF;
//WriteConfig = 0001100000001100

#define DRV8301WRITE		(0x00 << 15)
#define DRV8301READ			(0x01 << 15)

#define DRV8301ADD0			(0x00 << 11)
#define DRV8301ADD1			(0x01 << 11)
#define DRV8301ADD2			(0x02 << 11)
#define DRV8301ADD3			(0x03 << 11)




#define DRV8301_SPI_Enable() 			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define DRV8301_SPI_Disable() 			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)

#define DRV8301_Enable() 				HAL_GPIO_WritePin(En_Gate_GPIO_Port, En_Gate_Pin, GPIO_PIN_SET)
#define DRV8301_Disable() 				HAL_GPIO_WritePin(En_Gate_GPIO_Port, En_Gate_Pin, GPIO_PIN_RESET)


/*
	1，驱动芯片的PVDD必须接上（6V以上）
	2，EN_GATE必须使能：高电平
	3，SPI时序类型
	4，芯片启动要给delay()
*/
//uint8_t spi1_read_write_byte(uint8_t txdata);

void Drv8301_Init(void);



#endif


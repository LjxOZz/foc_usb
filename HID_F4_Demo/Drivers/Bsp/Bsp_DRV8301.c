#include "Bsp_DRV8301.h"
#include "Bsp_Uart.h"

#include "spi.h"

void spi1_write_byte(uint16_t txdata);
void spi1_read_byte(uint16_t *ReadConfig);


uint16_t Drv8301_ReadID(void)
{
	uint16_t ID;
	
	return ID;
}


void Drv8301_Init(void)
{
	uint16_t WriteConfig;
	uint16_t ReadConfig;
	
	
	//EN_GATE
	DRV8301_Enable();
	HAL_Delay(500);
	
	printf("EN_GATE SET\n");
	
	//ADD2 SET
	WriteConfig = DRV8301WRITE | DRV8301ADD2 | GATE_CURRENT | GATE_RESET | PWM_MODE | OCP_MODE | (17 << 6);//0001 0000 0001 0001  //OC_ADJ_SET
	spi1_write_byte(WriteConfig);
	printf("W_C16:%d\n", WriteConfig);
	WriteConfig = DRV8301READ | DRV8301ADD2;
	spi1_write_byte(WriteConfig);
	spi1_read_byte(&ReadConfig);
	printf("R_C16:%d\n", ReadConfig);

//	WriteConfig = DRV8301WRITE | DRV8301ADD3 | OCTW_MODE | GAINVALUE | DC_CAL_CH1 | DC_CAL_CH2 | OC_TOFF;
	WriteConfig = 6156;
	spi1_write_byte(WriteConfig);
	printf("W_C16:%d\n", WriteConfig);
	WriteConfig = DRV8301READ | DRV8301ADD3;
	spi1_write_byte(WriteConfig);
	spi1_read_byte(&ReadConfig);
	printf("R_C16:%d\n", ReadConfig);

}



/**
 * @brief       SPI1读写一个字节数据
 * @param       txdata  : 要发送的数据(2字节)
 * @retval      void
 */
void spi1_write_byte(uint16_t txdata)
{
	uint8_t w_config[3];
	w_config[0] = (txdata >> 8) & 0xFF; //高8位
	w_config[1] =  txdata       & 0xFF; //低8位

	
//	printf("W_C8:%d,%d\n", w_config[0],w_config[1]);

	DRV8301_SPI_Enable();
	HAL_SPI_Transmit(&hspi1, w_config, 2, 1000);
	DRV8301_SPI_Disable();
	
}

/**
 * @brief       SPI1读一个字节数据
 * @param       接收到的数据(2字节)
 * @retval      void
 */
void spi1_read_byte(uint16_t *ReadConfig)
{
    uint8_t rxdata[3];
	
	DRV8301_SPI_Enable();
	HAL_SPI_Receive(&hspi1, rxdata, 2, 1000);
	DRV8301_SPI_Disable();
	
	*ReadConfig = (rxdata[0] << 8) | rxdata[1];
	
}


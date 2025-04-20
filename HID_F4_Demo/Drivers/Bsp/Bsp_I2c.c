#include "Bsp_I2c.h"
#include "i2c.h"


uint16_t AS5600_I2c_Read(void)
{
	uint16_t jiaodu = 0;
	uint8_t I2C_Buffer_Read[4]={0};
	
	HAL_I2C_Mem_Read(&hi2c1, 0x6c, 0x0C, I2C_MEMADD_SIZE_8BIT, I2C_Buffer_Read,4,50);
	/*12位的数据处理成 角度数据*/
	jiaodu = I2C_Buffer_Read[0] << 8;		
	jiaodu = jiaodu | I2C_Buffer_Read[1];
	jiaodu = jiaodu * 0.08789;
	
	return jiaodu;
}
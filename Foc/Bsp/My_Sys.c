#include "My_Sys.h"
#include "Bsp_I2c.h"
#include "Foc.h"



/*

1，电角度 = 磁极对数x机械角度
2，磁极对数 =  7.0f 
3，


*/
uint8_t OpenLoop_Foc()
{
	uint16_t Mechanical_angle;				//机械角度
	uint16_t Electrical_angle;				//电角度
	
	Mechanical_angle = AS5600_I2c_Read();
	Electrical_angle = PolePair * Mechanical_angle;
	
	return 1;
}



#include "My_Sys.h"
#include "Bsp_I2c.h"
#include "Foc.h"



/*

1����Ƕ� = �ż�����x��е�Ƕ�
2���ż����� =  7.0f 
3��


*/
uint8_t OpenLoop_Foc()
{
	uint16_t Mechanical_angle;				//��е�Ƕ�
	uint16_t Electrical_angle;				//��Ƕ�
	
	Mechanical_angle = AS5600_I2c_Read();
	Electrical_angle = PolePair * Mechanical_angle;
	
	return 1;
}



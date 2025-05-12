#include "bsp_key.h"
extern uint8_t Coder_State;



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{
	//���������
	if(GPIO_Pin== Coder_DI_GPIO_PIN)
	{
		if(Coder_CLK_Input()==Coder_OFF) //DI���½���˳ʱ��
			Coder_State=Ready_Clock;
		else if(Coder_CLK_Input()==Coder_ON)
		{
			if(Coder_State==Ready_Clock)
				Coder_State=Down_Clock;
			else if(Coder_State==Ready_AntiClock)
				Coder_State=Down_AntiClock;
		}
	}
	else if(GPIO_Pin == Coder_CLK_GPIO_PIN)
	{
		if(Coder_DI_Input()==Coder_OFF) //DI���½�����ʱ��
			Coder_State=Ready_AntiClock;
		else if(Coder_DI_Input()==Coder_ON) //������ת
		{
			if(Coder_State==Ready_Clock)
				Coder_State=Down_Clock;
			else if(Coder_State==Ready_AntiClock)
				Coder_State=Down_AntiClock;
		}
	}
	


}
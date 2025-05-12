#include "Foc.h"
#include "arm_math.h"
#include "tim.h"
#include "usart.h"



void RevPack(float vd, float vq, float theta, float *valpha,float *vbeta)
{
	*valpha = vd * arm_cos_f32(theta) - vq * arm_sin_f32(theta);
	*vbeta = vd * arm_sin_f32(theta) + vq * arm_cos_f32(theta);
}  

/*
	
return: 3,1,5,4,6,2
		1,2,3,4,5,6
 */
uint8_t SectorJudge(float valpha, float vbeta)
{
	float A =  vbeta;
	float B =  sqrt3/2.0f*valpha - 0.5f*vbeta;
	float C = -sqrt3/2.0f*valpha - 0.5f*vbeta;
	
	uint8_t N = 0;
	if(A > 0)
		N = N + 1;
	if(B > 0)
		N = N + 2;
	if(C > 0)
		N = N + 4;
	
	return N;
}

void V_Time(uint8_t N, float Valpha, float Vbeta, float Udc, float Tpwm, float *Ta, float *Tb)
{
	float X = (sqrt3*Tpwm) / Udc * Vbeta;                                                    
	float Y = (sqrt3*Tpwm) / Udc * ((sqrt3 / 2.0f) * Valpha - (0.5f) * Vbeta); 
	float Z = (sqrt3*Tpwm) / Udc * ((sqrt3 / 2.0f) * Valpha + (0.5f) * Vbeta);

	if(N == 3){
		*Ta = Y;
		*Tb = X;
	}
	if(N == 1){
		*Tb = Z;   
		*Ta = -Y;
	}
	if(N == 5){
		*Ta = X;
		*Tb = -Z;
	}
	if(N == 4){
		*Tb = -Y;   
		*Ta = -X;
	}
	if(N == 6){
		*Ta = -Z;
		*Tb = Y;
	}
	if(N == 2){
		*Tb = -X;   
		*Ta = Z;
	}
	
}

void CCR_ABC_Value(uint8_t N, float Ta, float Tb, uint16_t Tpwm, uint16_t *CCR1, uint16_t *CCR2, uint16_t *CCR3)
{
		
	float temp = Ta + Tb;
	if(temp > Tpwm){
		Ta = Ta / temp * Tpwm;
		Tb = Tb / temp * Tpwm;
	}
	float Value1 = (Tpwm - Ta - Tb) / 4.0f;
	float Value2 = Value1 + Ta / 2.0f;
	float Value3 = Value2 + Tb / 2.0f;

	switch(N){
		case 3:
				*CCR1 = Value1;
				*CCR2 = Value2;
				*CCR3 = Value3;
				break;
		case 1:
				*CCR1 = Value2;
				*CCR2 = Value1;
				*CCR3 = Value3;
				break;
		case 5:
				*CCR1 = Value3;
				*CCR2 = Value1;
				*CCR3 = Value2;
				break;
		case 4:
				*CCR1 = Value3;
				*CCR2 = Value2;
				*CCR3 = Value1;
				break;
		case 6:
				*CCR1 = Value2;
				*CCR2 = Value3;
				*CCR3 = Value1;
				break;
		case 2:
				*CCR1 = Value1;
				*CCR2 = Value3;
				*CCR3 = Value2;
				break;
	}
	
}


//0,3,
void Svpwm_ABC(float Vd, float Vq, float theta, float Udc, uint16_t Tpwm)
{
	
	float Valpha = 0;
	float Vbeta = 0;

	RevPack(Vd, Vq, theta, &Valpha, &Vbeta);
	uint8_t N = SectorJudge(Valpha, Vbeta);
	float Ta;
	float Tb;
	V_Time(N, Valpha, Vbeta, Udc, Tpwm, &Ta, &Tb);
	uint16_t CCR1;
	uint16_t CCR2;
	uint16_t CCR3;
	CCR_ABC_Value(N, Ta, Tb, Tpwm, &CCR1, &CCR2, &CCR3);
//	printf("N:%d\n", N);
//	printf("CCR:%d,%d,%d\n", CCR1, CCR2, CCR3);
	
//	float CCR[3];
//	uint8_t temp[16] = {0,0,0,0,
//						 0,0,0,0,
//						 0,0,0,0,
//						 0,0,0x80,0x7f};//
//	CCR[0] = (float)(CCR1);
//	CCR[1] = (float)(CCR2);
//	CCR[2] = (float)(CCR3);
//	memcpy(temp , (uint8_t *)CCR ,sizeof(CCR));
//	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)temp, 4*4);
	
	// write Time1
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, CCR1);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, CCR2);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, CCR3);
	
}


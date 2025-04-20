#include "Bsp_Adc.h"
#include "adc.h"

uint16_t dmaDataBuffer[BATCH_DATA_LEN];

// 多通道ADC采集
// multi channel DMA
void DMA_ADC_Init(ADC_HandleTypeDef* hadc)
{

	HAL_ADC_Start_DMA(hadc, (uint32_t *)dmaDataBuffer ,BATCH_DATA_LEN);
	
}



//
// single_channel 
//uint16_t ADC_Read(ADC_HandleTypeDef* hadc)
//{
//		HAL_ADC_Start(hadc);
//		HAL_ADC_PollForConversion(hadc, 500);
//		if(HAL_IS_BIT_SET(HAL_ADC_GetState(hadc), HAL_ADC_STATE_REG_EOC)){
//				return HAL_ADC_GetValue(hadc);
//		}
//		return 0xFFFF;
//}




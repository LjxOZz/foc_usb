#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "main.h"

#define		BATCH_DATA_LEN		7

/*
	Scan_ADC+DMA
	min:						3.3V/4096
	time:						21Mhz
	ADC_interrupt: 	no
	Trigger_Source: Soft(DMA_ADC_Init)
*/


void DMA_ADC_Init(ADC_HandleTypeDef* hadc);



#endif


/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "Foc.h"
#include "Bsp_I2c.h"
#include "Bsp_Adc.h"
#include "Bsp_DRV8301.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define PI 		3.14159265358979f

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

//ADC
extern uint16_t dmaDataBuffer[BATCH_DATA_LEN];
int ADC_Value;

//RX
uint8_t aRxBuffer[3];
//TX
float Ivuw_temp[3];
float Iabc_temp[3];
uint8_t temp_Data[16] = {0,0,0,0,
						 0,0,0,0,
						 0,0,0,0,
						 0,0,0x80,0x7f};//改成结构体

uint16_t Angle;

//SVPWM
float Vd=0;
float Vq=1;
float theta = 0;
float Vbus;
uint16_t Tpwm = 8400;

uint32_t CCR1;
uint32_t CCR2;
uint32_t CCR3;

//FOC_Speed
int FOC_Speed = 120;		//   单位 r/min
float One_rad;
int ARR7;


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USB_DEVICE_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  
  	HAL_Delay(500);
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);	//Uart	
	Drv8301_Init();											//Drv8301
	
	FOC_Speed = 30;
	One_rad = 60.0f / (float)FOC_Speed * 1000.0f;
	ARR7 = (int) (One_rad / 7.0f / 3600.0f * 1000.0f * 84.0f);
	TIM7->ARR = ARR7;
	
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
	
	
	DMA_ADC_Init(&hadc1);									//Adc
	HAL_TIM_Base_Start_IT(&htim7);							//Interrupt


//	HAL_Delay(500);
//    while(1){
//		
//		
//		HAL_Delay(1000);
//		DMA_ADC_Init(&hadc1);									//Adc
//		
//	}

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{

//	printf("Iabc:%d,%d,%d\n",(dmaDataBuffer[4]),(dmaDataBuffer[5]),(dmaDataBuffer[6]));太慢了
	
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

	
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
    if (htim->Instance == TIM4){

	}
	
	if (htim->Instance == TIM7){
		
		Vbus = (float)(dmaDataBuffer[3])*8.336f/4095.0f*3.3f;
		if(Vbus > 3500)Vbus = 3500;
		
		Svpwm_ABC(0, 1, theta, 12, Tpwm);
		theta = theta + 0.001745329f;	
		if(theta > 6.2831852f ){ theta = 0.0f;}
		
//		Ivuw_temp[0] = (float)(dmaDataBuffer[0])/4095.0f*3.3f*8.336f;			
//		Ivuw_temp[1] = (float)(dmaDataBuffer[1])/4095.0f*3.3f*8.336f;
//		Ivuw_temp[2] = (float)(dmaDataBuffer[2])/4095.0f*3.3f*8.336f;
//		memcpy(temp_Data , (uint8_t *)Ivuw_temp ,sizeof(Ivuw_temp));
//		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)temp_Data, 4*4);
//		
		Iabc_temp[0] = (float)(dmaDataBuffer[4]/4095.0f*3.3f);
		Iabc_temp[1] = (float)(dmaDataBuffer[5]/4095.0f*3.3f);
		Iabc_temp[2] = (float)(dmaDataBuffer[6]/4095.0f*3.3f);
		memcpy(temp_Data , (uint8_t *)Iabc_temp ,sizeof(Iabc_temp));
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)temp_Data, 4*4);
	
		
	}
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

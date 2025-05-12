/* USER CODE BEGIN Header */
/**
  ******************************************************************************
	一. 程序结构说明
	此程序在只调用了USB外设的PA11和PA12引脚和几个按键输入引脚(bsp_key.c)，对于STM32F103，只需要修改bsp_key中的
	引脚定义和gpio.c中的引脚初始化即可，LED可以不用。若用CubeMX重新覆盖此程序，则会将描述符覆盖掉，需要重新添加描述符，步骤如下。若想避免的话覆盖
	自己的HID，在USB外设中，选自定义HID设备即可。
	
	
	二. 如何移植程序
	使用CubeMX生成，添加USB库，初始化代码后，需要做如下操作
	1. 修改HID报告描述符：在usbd_desc.c文件中找到数组 HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE]即为
	HID报告描述的存放位置，初始化程序的描述符仅实现了鼠标的功能，可以删掉，用此程序描述符替换。HID报告描述符
	事先定义了我们要给电脑发送的数据的格式，好让电脑识别，也可以根据需要自己修改描述符。
	2. 替换完成后，需要修改描述符数组的大小，在usbd_hid.h中找到HID_MOUSE_REPORT_DESC_SIZE定义的地方，将其
	修改为我们自己的报告描述符的大小，此程序的描述的大小为176字节。同时要修改发送一次数据字节的长度，在usbd_hid.h中
	的HID_EPIN_SIZE，具体大小根据报告描述符定义的格式计算出，这个不包含ReportID所占用的一个字节。本例中
	为8(键盘)+1(多媒体)+4(鼠标)=13字节。
	3. usbd_hid.c中函数USBD_HID_CfgFSDesc()中，需要将nInterfaceProtocol项目改位0x01，这样插入电脑后，识别到的即为
	键盘，不改的话识别为鼠标。usbd_conf.h中的HID_FS_BINTERVAL可设置设置电脑轮询的时间间隔，单位为ms，此程序设置为1，
	两个发送函数之间的时间间隔尽量大于此时间
	
	
	三. 其他注意事项
	经过多次测试，发生数据的格式必须如下，以发送a为例
		buff=a的键值  //赋值操作
		发送数据      //表示按下按键a
		delayms
		buff =0     //清空数值
		发送数据    //发送空的值，表示抬起按键a
		
		注意：两个发送数据的延时一定要有，尽量大于设定的轮询间隔，延时时间过小容易卡死。
		发送a的值后，要跟着发送一个空值，表示抬起，否则就会出现长按的现象
	
	对于EC11旋转编码器，定义了2个引脚，采用检测下降沿中的方式来确定旋转，模块自带有滤波电容，没有做软件消抖处理。经过测试
	几乎不会发送丢步现象。
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t USBD_HID_SendReport (USBD_HandleTypeDef *pdev, 
                                 uint8_t *report,
                                 uint16_t len);
#define BUFF_KEY_SIZE 9
#define BUFF_Audio_SIZE 2
#define BUFF_Mouse_SIZE 5

//键盘发送数据
//字节1为ReportID:1，字节2为功能键盘，每一位对应一个按键，
/*  更详细的规定参考文档Device Class Definition for HID的8.3节
					Bit Key 
					0 LEFT CTRL
					1 LEFT SHIFT
					2 LEFT ALT
					3 LEFT GUI 
					4 RIGHT CTRL
					5 RIGHT SHIFT
					6 RIGHT ALT
					7 RIGHT GUI
*/
//字节3保留，字节4-9表示表示按键键值，
//每个按键对于的值可在HID Usage Tables文档中的第10节查询
uint8_t buff_Key[9]={0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


//多媒体发送数组，根据HID报告描述符的定义，第一个字节位ReportID:2,
//第二个字节从高到低，每一位分别对应：
/*	      Bit 功能 
					0   音量升高
					1   音量减小
					2   静音
					3   播放/暂停
					4   暂停
					5   下一首
					6   上一首
					7   退出播放
*/
//更多的操作在HID描述符中添加，参考HID Usage Tables的15节Consumer Page
//0x01即表示音量升高
uint8_t buff_Audio[2]={0x02,0x00};


//鼠标发送数组
//第一个字为ReportID:3，字节2的0000 0xxx后三位分别表示中间单击，右键单击，左键单机
//字节3为x轴移动距离，字节4为y轴移动距离，字节5为滚轮移动值 其范围为-127~127
uint8_t buff_Mouse[5]={0x03,0x00,0x00,0x00,0x00};



//后续可以都写成结构体的形式，赋值更方便
//这些数据每一位的定义可在usbd_hid.c中HID_MOUSE_ReportDescs规定，可自行修改

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

//HID数据发送函数
void Send_USB_Buff(uint8_t *buff,uint8_t size)
{
	USBD_HID_SendReport(&hUsbDeviceFS, buff, size);
	HAL_Delay(1);   //两个相邻的上报函数之间一定要有延时，否则无法发送
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//编码器的状态，中断函数检测编码器引脚变化来改变状态
uint8_t Coder_State=Static_State;  

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
  MX_USB_DEVICE_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
  /* USER CODE END 2 */
	
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		//下面展示了三种功能，由于我的开发板只有2个按键，因此只能分别注释掉，单独测试每一部分
		//按键多的话，可以全部打开
/*****************************************/
		//1. 鼠标控制 
		/*
		if(KEY1_Input()==KEY_ON) //按键1按下
		{
			LED0_ON();
		//下面三组可选择一组测试
			buff_Mouse[2] = -3; //鼠标左移3个单位
			//buff_Mouse[1] = 0x01; //左键点击 0000 0001 
			//buff_Mouse[4]= 2;     //滚轮上移2个单位
		}
		else
		{
			LED0_OFF();
		}
		
		if(KEY2_Input()==KEY_ON) //按键2按下
		{
			LED1_ON();
			buff_Mouse[2] = 3;  //鼠标右移3个单位
			//buff_Mouse[1] = 0x02; //右键点击; 0000 0010
			//buff_Mouse[4]= -2;     //滚轮下
		}
		else
		{
			LED1_OFF();
		}
		

		Send_USB_Buff(buff_Mouse,BUFF_Mouse_SIZE); //上报鼠标数据
		HAL_Delay(1);  //移动过快可以增加此处的延时  两个上报函数之间一定要有延时，否则无法发送
		buff_Mouse[2]=0; //移动数据清零
		//buff_Mouse[1] = 0x00; //按键数据清零
		//buff_Mouse[4]= 0;     //滚轮数据清零
		
		*/
/*****************************************/		
		//2.按键输入
		/*
		if(KEY1_Input()==KEY_ON) 
		{
			LED0_ON();
			buff_Key[3]=0x04; //输入a
			//buff_Key[1]=0x01 //左Ctrl
			//buff_Key[4] = 0x06 //输入C
			//一次报文可发送最多6个按键，若想要实现一次发送所有按键的状态，需要使用Bit Map来定义HID
		}
		else
		{
			LED0_OFF();
		}
		
		if(KEY2_Input()==KEY_ON) 
		{
			LED1_ON();
			buff_Key[4]=0x05; //输入b
		}
		else
		{
			LED1_OFF();
		}
		Send_USB_Buff(buff_Key,BUFF_KEY_SIZE); //上报按键数据
		HAL_Delay(1);
		//清楚按键数据，不然会一直发送
		buff_Key[3]=0x00;
		buff_Key[4]=0x00;
		*/
		
/*****************************************/		
		//3. 多媒体控制
		
		if(KEY1_Input()==KEY_ON) 
		{
			LED0_ON();
			//buff_Audio[1]= 0x01; //音量增
			buff_Audio[1] = 0x20; //下一首 0010 0000  按键按下后，保持的时间较长，循环会多次检测到，倒是
		}
		else
		{
			LED0_OFF();
		}
		
		if(KEY2_Input()==KEY_ON) 
		{
			LED1_ON();
			//buff_Audio[1] |= 0x02; //音量减
			buff_Audio[1] |= 0x08; //开始/暂停 0000 1000
		}
		else
		{
			LED1_OFF();
		}
		
		//旋钮的检测
		//编码器状态的检测在中断中进行
		
		if(Coder_State==Down_Clock ) //旋钮顺时针旋转
		{
			LED0_Toggle();
			buff_Audio[1] |= 0x01; //音量增
			Coder_State=Static_State;  //状态清零 注意，这个一定要在if内，在循环中会影响中断的进行
		}
		else if(Coder_State==Down_AntiClock) //旋钮逆时针旋转
		{
			LED0_Toggle();
			buff_Audio[1] |= 0x02;  //音量减	
			Coder_State=Static_State;
		}
		
		  
		
		Send_USB_Buff(buff_Audio,BUFF_Audio_SIZE); //上报按键数据
		HAL_Delay(15);//由于按键按下后保持低电平时间较长，循环时间短，多次检测到按键，导致多次发送指令，无法实现精确控制
										//增加此延时时间可缓解此现象，但建议在测试时这样改，实际项目中建议采用单独的定时器中断的方式来检测
		buff_Audio[1]=0x00; // 数据清零
		
/*****************************************/	
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

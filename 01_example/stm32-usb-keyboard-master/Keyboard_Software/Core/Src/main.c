/* USER CODE BEGIN Header */
/**
  ******************************************************************************
	һ. ����ṹ˵��
	�˳�����ֻ������USB�����PA11��PA12���źͼ���������������(bsp_key.c)������STM32F103��ֻ��Ҫ�޸�bsp_key�е�
	���Ŷ����gpio.c�е����ų�ʼ�����ɣ�LED���Բ��á�����CubeMX���¸��Ǵ˳�����Ὣ���������ǵ�����Ҫ����������������������¡��������Ļ�����
	�Լ���HID����USB�����У�ѡ�Զ���HID�豸���ɡ�
	
	
	��. �����ֲ����
	ʹ��CubeMX���ɣ����USB�⣬��ʼ���������Ҫ�����²���
	1. �޸�HID��������������usbd_desc.c�ļ����ҵ����� HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE]��Ϊ
	HID���������Ĵ��λ�ã���ʼ���������������ʵ�������Ĺ��ܣ�����ɾ�����ô˳����������滻��HID����������
	���ȶ���������Ҫ�����Է��͵����ݵĸ�ʽ�����õ���ʶ��Ҳ���Ը�����Ҫ�Լ��޸���������
	2. �滻��ɺ���Ҫ�޸�����������Ĵ�С����usbd_hid.h���ҵ�HID_MOUSE_REPORT_DESC_SIZE����ĵط�������
	�޸�Ϊ�����Լ��ı����������Ĵ�С���˳���������Ĵ�СΪ176�ֽڡ�ͬʱҪ�޸ķ���һ�������ֽڵĳ��ȣ���usbd_hid.h��
	��HID_EPIN_SIZE�������С���ݱ�������������ĸ�ʽ����������������ReportID��ռ�õ�һ���ֽڡ�������
	Ϊ8(����)+1(��ý��)+4(���)=13�ֽڡ�
	3. usbd_hid.c�к���USBD_HID_CfgFSDesc()�У���Ҫ��nInterfaceProtocol��Ŀ��λ0x01������������Ժ�ʶ�𵽵ļ�Ϊ
	���̣����ĵĻ�ʶ��Ϊ��ꡣusbd_conf.h�е�HID_FS_BINTERVAL���������õ�����ѯ��ʱ��������λΪms���˳�������Ϊ1��
	�������ͺ���֮���ʱ�����������ڴ�ʱ��
	
	
	��. ����ע������
	������β��ԣ��������ݵĸ�ʽ�������£��Է���aΪ��
		buff=a�ļ�ֵ  //��ֵ����
		��������      //��ʾ���°���a
		delayms
		buff =0     //�����ֵ
		��������    //���Ϳյ�ֵ����ʾ̧�𰴼�a
		
		ע�⣺�����������ݵ���ʱһ��Ҫ�У����������趨����ѯ�������ʱʱ���С���׿�����
		����a��ֵ��Ҫ���ŷ���һ����ֵ����ʾ̧�𣬷���ͻ���ֳ���������
	
	����EC11��ת��������������2�����ţ����ü���½����еķ�ʽ��ȷ����ת��ģ���Դ����˲����ݣ�û�����������������������
	�������ᷢ�Ͷ�������
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

//���̷�������
//�ֽ�1ΪReportID:1���ֽ�2Ϊ���ܼ��̣�ÿһλ��Ӧһ��������
/*  ����ϸ�Ĺ涨�ο��ĵ�Device Class Definition for HID��8.3��
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
//�ֽ�3�������ֽ�4-9��ʾ��ʾ������ֵ��
//ÿ���������ڵ�ֵ����HID Usage Tables�ĵ��еĵ�10�ڲ�ѯ
uint8_t buff_Key[9]={0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


//��ý�巢�����飬����HID�����������Ķ��壬��һ���ֽ�λReportID:2,
//�ڶ����ֽڴӸߵ��ͣ�ÿһλ�ֱ��Ӧ��
/*	      Bit ���� 
					0   ��������
					1   ������С
					2   ����
					3   ����/��ͣ
					4   ��ͣ
					5   ��һ��
					6   ��һ��
					7   �˳�����
*/
//����Ĳ�����HID����������ӣ��ο�HID Usage Tables��15��Consumer Page
//0x01����ʾ��������
uint8_t buff_Audio[2]={0x02,0x00};


//��귢������
//��һ����ΪReportID:3���ֽ�2��0000 0xxx����λ�ֱ��ʾ�м䵥�����Ҽ��������������
//�ֽ�3Ϊx���ƶ����룬�ֽ�4Ϊy���ƶ����룬�ֽ�5Ϊ�����ƶ�ֵ �䷶ΧΪ-127~127
uint8_t buff_Mouse[5]={0x03,0x00,0x00,0x00,0x00};



//�������Զ�д�ɽṹ�����ʽ����ֵ������
//��Щ����ÿһλ�Ķ������usbd_hid.c��HID_MOUSE_ReportDescs�涨���������޸�

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

//HID���ݷ��ͺ���
void Send_USB_Buff(uint8_t *buff,uint8_t size)
{
	USBD_HID_SendReport(&hUsbDeviceFS, buff, size);
	HAL_Delay(1);   //�������ڵ��ϱ�����֮��һ��Ҫ����ʱ�������޷�����
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//��������״̬���жϺ��������������ű仯���ı�״̬
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
		
		//����չʾ�����ֹ��ܣ������ҵĿ�����ֻ��2�����������ֻ�ֱܷ�ע�͵�����������ÿһ����
		//������Ļ�������ȫ����
/*****************************************/
		//1. ������ 
		/*
		if(KEY1_Input()==KEY_ON) //����1����
		{
			LED0_ON();
		//���������ѡ��һ�����
			buff_Mouse[2] = -3; //�������3����λ
			//buff_Mouse[1] = 0x01; //������ 0000 0001 
			//buff_Mouse[4]= 2;     //��������2����λ
		}
		else
		{
			LED0_OFF();
		}
		
		if(KEY2_Input()==KEY_ON) //����2����
		{
			LED1_ON();
			buff_Mouse[2] = 3;  //�������3����λ
			//buff_Mouse[1] = 0x02; //�Ҽ����; 0000 0010
			//buff_Mouse[4]= -2;     //������
		}
		else
		{
			LED1_OFF();
		}
		

		Send_USB_Buff(buff_Mouse,BUFF_Mouse_SIZE); //�ϱ��������
		HAL_Delay(1);  //�ƶ�����������Ӵ˴�����ʱ  �����ϱ�����֮��һ��Ҫ����ʱ�������޷�����
		buff_Mouse[2]=0; //�ƶ���������
		//buff_Mouse[1] = 0x00; //������������
		//buff_Mouse[4]= 0;     //������������
		
		*/
/*****************************************/		
		//2.��������
		/*
		if(KEY1_Input()==KEY_ON) 
		{
			LED0_ON();
			buff_Key[3]=0x04; //����a
			//buff_Key[1]=0x01 //��Ctrl
			//buff_Key[4] = 0x06 //����C
			//һ�α��Ŀɷ������6������������Ҫʵ��һ�η������а�����״̬����Ҫʹ��Bit Map������HID
		}
		else
		{
			LED0_OFF();
		}
		
		if(KEY2_Input()==KEY_ON) 
		{
			LED1_ON();
			buff_Key[4]=0x05; //����b
		}
		else
		{
			LED1_OFF();
		}
		Send_USB_Buff(buff_Key,BUFF_KEY_SIZE); //�ϱ���������
		HAL_Delay(1);
		//����������ݣ���Ȼ��һֱ����
		buff_Key[3]=0x00;
		buff_Key[4]=0x00;
		*/
		
/*****************************************/		
		//3. ��ý�����
		
		if(KEY1_Input()==KEY_ON) 
		{
			LED0_ON();
			//buff_Audio[1]= 0x01; //������
			buff_Audio[1] = 0x20; //��һ�� 0010 0000  �������º󣬱��ֵ�ʱ��ϳ���ѭ�����μ�⵽������
		}
		else
		{
			LED0_OFF();
		}
		
		if(KEY2_Input()==KEY_ON) 
		{
			LED1_ON();
			//buff_Audio[1] |= 0x02; //������
			buff_Audio[1] |= 0x08; //��ʼ/��ͣ 0000 1000
		}
		else
		{
			LED1_OFF();
		}
		
		//��ť�ļ��
		//������״̬�ļ�����ж��н���
		
		if(Coder_State==Down_Clock ) //��ť˳ʱ����ת
		{
			LED0_Toggle();
			buff_Audio[1] |= 0x01; //������
			Coder_State=Static_State;  //״̬���� ע�⣬���һ��Ҫ��if�ڣ���ѭ���л�Ӱ���жϵĽ���
		}
		else if(Coder_State==Down_AntiClock) //��ť��ʱ����ת
		{
			LED0_Toggle();
			buff_Audio[1] |= 0x02;  //������	
			Coder_State=Static_State;
		}
		
		  
		
		Send_USB_Buff(buff_Audio,BUFF_Audio_SIZE); //�ϱ���������
		HAL_Delay(15);//���ڰ������º󱣳ֵ͵�ƽʱ��ϳ���ѭ��ʱ��̣���μ�⵽���������¶�η���ָ��޷�ʵ�־�ȷ����
										//���Ӵ���ʱʱ��ɻ�������󣬵������ڲ���ʱ�����ģ�ʵ����Ŀ�н�����õ����Ķ�ʱ���жϵķ�ʽ�����
		buff_Audio[1]=0x00; // ��������
		
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

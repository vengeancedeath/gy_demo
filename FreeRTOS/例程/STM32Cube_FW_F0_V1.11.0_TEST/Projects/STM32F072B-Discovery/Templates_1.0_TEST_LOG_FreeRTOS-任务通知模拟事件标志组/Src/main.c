/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "gpio.h"

#include <stdio.h>
#include <stdbool.h>

// Include headfiles for VCP
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "logger.h"
#include "FreeRTOS.h"
#include "task.h"
#include "logger.h"

#define USE_STATIC_TASK	0 // 是否使用静态任务
#define USE_DYNAMIC_TASK	0 // 是否使用动态任务
#define USE_SUSPEND_RESUME_TASK	0 // 是否使用挂起恢复任务
#define USE_QUEUE_TASK	0 // 是否使用队列任务
#define USE_SEMAPHORE_TASK	0 // 是否使用信号量任务
#define USE_SEMAPHORE_COUNTING_TASK	0 // 是否使用信号量计数任务
#define USE_TASK_NOTI_SEMAPHORE_TASK	0 // 是否使用任务通知模拟信号量任务
#define USE_TASK_NOTI_QUEUE_TASK	0 // 是否使用任务通知模拟消息队列任务
#define USE_TASK_NOTI_EVENT_GROUPS_TASK	1 // 是否使用任务通知模拟事件标志组任务


#if USE_STATIC_TASK
#include "freertos_task_static.h"
#endif
#if USE_DYNAMIC_TASK
#include "freertos_task.h"
#endif
#if USE_SUSPEND_RESUME_TASK
#include "freertos_task_suspend_resume.h"
#endif
#if USE_QUEUE_TASK
#include "freertos_queue.h"
#endif
#if USE_SEMAPHORE_TASK
#include "freertos_semphr.h"
#endif
#if USE_SEMAPHORE_COUNTING_TASK
#include "freertos_semphr_counting.h"
#endif
#if USE_TASK_NOTI_SEMAPHORE_TASK
#include "freertos_task_noti_semphr.h"
#endif
#if USE_TASK_NOTI_QUEUE_TASK
#include "freertos_task_noti_queue.h"
#endif
#if USE_TASK_NOTI_EVENT_GROUPS_TASK
#include "freertos_task_noti_event_groups.h"
#endif



#define printf	USB_Printf

static const char *log_tag = "main";
#define MAIN_INFO(format, ...) LOG_INFO(log_tag, format, ##__VA_ARGS__)

volatile USER_CDC_PACKAGE_TYPE_t user_cdc_buffer; // it needs to declared again in main.c

/** @addtogroup STM32F0xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* Private functions ---------------------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//uint8_t User_CDC_Buf[8] ={0}; // This buffer used to copy the data from VCP and command check & unpack
//Eric_H:026412
//uint8_t User_CDC_Buf[12] ={0}; // This buffer used to copy the data from VCP and command check & unpack
//Eric_H:026414
uint8_t User_CDC_Buf[22] ={0}; // The max number should be "2 + 1 + 1 + 1 + 1 + 16" bytes to receive complete 16 bytes
uint8_t uI2C_Read2PC = 0; // Used to store the data which read back from I2C
uint8_t readData = 0;
/* USER CODE END 0 */

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t *ptruRX_Buffer; //Declared the pointer for receive buffer

  /* USER CODE END 1 */

  /* STM32F0xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Low Level Initialization
     */
	 

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock to have a system clock = 48 Mhz */
  SystemClock_Config();


  /* Add your application code here
     */
  /* USER CODE BEGIN SysInit */
  USB_Status_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  

	logger_init(LOG_LEVEL_INFO,LOG_OUTPUT_UART);
	HAL_Delay(4000);
  //创建任务
#if USE_STATIC_TASK
	 freertos_start_static();
#endif
#if USE_DYNAMIC_TASK
	 freertos_start();
#endif
#if USE_SUSPEND_RESUME_TASK
	 freertos_start_suspend_resume();
#endif
#if USE_QUEUE_TASK
	 freertos_start_queue();
#endif
#if USE_SEMAPHORE_TASK
	 freertos_start_semaphore();
#endif
#if USE_SEMAPHORE_COUNTING_TASK
	 freertos_start_semaphore_counting();
#endif  
#if USE_TASK_NOTI_SEMAPHORE_TASK
	 freertos_start_task_noti_semaphor();
#endif  
#if USE_TASK_NOTI_QUEUE_TASK
	 freertos_start_task_noti_queue();
#endif
#if USE_TASK_NOTI_EVENT_GROUPS_TASK
	 freertos_start_task_noti_event_groups();
#endif



  while (1)
  {


	  //User_CDC_Buf > 0 means receiving datas from USB
//	  if(user_cdc_buffer.len > 0){

//		  user_cdc_buffer.len = 0; // clear len to avoid infinity loop
//		  //Eric_H:026412
//		  memcpy(User_CDC_Buf, user_cdc_buffer.data, 8); // copy the user_cdc_buffer to User_CDC_Buf for command check

//		  ptruRX_Buffer = User_CDC_Buf; //point to CDC buffer

//		}

  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = HSI48 (Internal 48MHz oscillator)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            HSI State                      = ON (8MHz, for I2C1 peripheral)
  *            HSI48 State                    = ON (48MHz, for system clock & USB)
  *            PLL State                      = Disabled
  *            Flash Latency(WS)              = 1
  *            Peripheral Clock Configuration:
  *              - USB Clock Source           = HSI48
  *              - USART1 Clock Source        = PCLK1
  *              - I2C1 Clock Source          = HSI
  * @param  None
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; // HSI48 calibration value
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  MAIN_INFO("Error_Handler: HAL error return state");
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  /*while (1)
  {
  } */
}
#endif

/**
 * @brief  FreeRTOS栈溢出钩子函数，当任务栈溢出时由内核自动调用
 * @param  xTask: 溢出任务的句柄
 * @param  pcTaskName: 溢出任务的名称，用于定位问题任务
 * @retval 无
 * @note   由configCHECK_FOR_STACK_OVERFLOW=2启用，方法二会检测栈末尾填充值
 *         是否被覆盖，比方法一更可靠。进入此函数后系统已不可恢复，只能
 *         通过日志输出溢出任务名称后停机，便于调试定位问题
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	(void)xTask;
	(void)pcTaskName;
	taskDISABLE_INTERRUPTS();
  MAIN_INFO("vApplicationStackOverflowHook: %s overflow", pcTaskName);
	for(;;);
}

/**
 * @brief  FreeRTOS Tick中断钩子函数，每个Tick周期(2ms)由内核自动调用
 * @retval 无
 * @note   由configUSE_TICK_HOOK启用，当前为空实现。
 *         注意：此函数在中断上下文中执行，不可调用阻塞API
 *         (如vTaskDelay、xQueueReceive等)，执行时间应尽量短
 */
void vApplicationTickHook(void)
{

}

/**
 * @brief  FreeRTOS内存分配失败钩子函数，当pvPortMalloc无法分配所需内存时调用
 * @retval 无
 * @note   由configUSE_MALLOC_FAILED_HOOK启用。常见原因：
 *         1. configTOTAL_HEAP_SIZE设置过小，堆内存不足
 *         2. 任务栈过大或创建任务过多，超出堆容量
 *         3. 存在内存泄漏（heap_4方案下反复分配释放产生碎片）
 *         进入此函数后系统已不可恢复，停机便于调试定位
 */
void vApplicationMallocFailedHook(void)
{
	taskDISABLE_INTERRUPTS();
  MAIN_INFO("vApplicationMallocFailedHook: malloc failed");
 	for(;;);
}

/**
  * @}
  */

/**
  * @}
  */
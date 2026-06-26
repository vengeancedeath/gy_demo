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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Include headfiles for VCP
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "logger.h"
#include "open_bootloader.h"
#include "app_openbootloader.h"

static const char *log_tag = "main";
#define MAIN_BUSS_INFO(format, ...) LOG_INFO(log_tag, format, ##__VA_ARGS__)

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

uint8_t jump_flag = 0;


#define bootloader_st "start bootloader"
#define bootloader_to "start boot_to__A"
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t *ptruRX_Buffer; //Declared the pointer for receive buffer

  
  HAL_Init();

  SystemClock_Config();


  USB_Status_Init();

//  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();

	logger_init(LOG_LEVEL_INFO,LOG_OUTPUT_UART);


	
		MAIN_BUSS_INFO("this is pragram bootloader");
  
  while (1)
  {
		
		if(jump_flag == 0)
		{
		
			HAL_Delay(20);
			MAIN_BUSS_INFO("this is pragram bootloader");
			
			if(user_cdc_buffer.len > 0){

				user_cdc_buffer.len = 0; // clear len to avoid infinity loop
				//Eric_H:026412
				memcpy(User_CDC_Buf, user_cdc_buffer.data, 22); // copy the user_cdc_buffer to User_CDC_Buf for command check

				ptruRX_Buffer = User_CDC_Buf; //point to CDC buffer
	//			MAIN_BUSS_INFO("strncmp()A = %s",ptruRX_Buffer);
	//			MAIN_BUSS_INFO("strncmp()B = %s",bootloader_st);
				
				if(strncmp(ptruRX_Buffer,bootloader_st,16) == 0)
				{
					MAIN_BUSS_INFO("strncmp() = %d",strncmp(ptruRX_Buffer,bootloader_st,16));
					MAIN_BUSS_INFO("start bootloader");
//					jump_to_app();
					jump_flag = 1;
					
					MAIN_BUSS_INFO("this is pragram bootloader_ing");
					OpenBootloader_Init();
					MAIN_BUSS_INFO("OpenBootloader_Init");
				}
				else if(strncmp(ptruRX_Buffer,bootloader_to,16) == 0)
				{
					MAIN_BUSS_INFO("strncmp() = %d",strncmp(ptruRX_Buffer,bootloader_to,16));
					MAIN_BUSS_INFO("start boot_to__A");
  				jump_to_app();
					
				}
				else{
					MAIN_BUSS_INFO("strncmp() = %d",strncmp(ptruRX_Buffer,bootloader_st,16));
					MAIN_BUSS_INFO("start bootloader error");
				}
				
				
				
				

			}
		}
		else
		{
			OpenBootloader_ProtocolDetection();
		}

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
  * @}
  */

/**
  * @}
  */
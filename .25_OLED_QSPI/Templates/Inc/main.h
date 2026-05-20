/**
  ******************************************************************************
  * @file    Templates/Inc/main.h 
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Error_Handler(void);

#define EN_1V2_Pin GPIO_PIN_0
#define EN_1V2_GPIO_Port GPIOC

#define EN_3V3_Pin GPIO_PIN_2
#define EN_3V3_GPIO_Port GPIOC

#define EN_1V8_Pin GPIO_PIN_1
#define EN_1V8_GPIO_Port GPIOC

#define EN_ELVDD_Pin GPIO_PIN_0
#define EN_ELVDD_GPIO_Port GPIOC

#define EN_ELVSS_Pin GPIO_PIN_3
#define EN_ELVSS_GPIO_Port GPIOC

#define QSPI_DO0_Pin GPIO_PIN_9
#define QSPI_DO0_GPIO_Port GPIOC

#define QSPI_DO1_Pin GPIO_PIN_10
#define QSPI_DO1_GPIO_Port GPIOC

#define QSPI_DO2_Pin GPIO_PIN_8
#define QSPI_DO2_GPIO_Port GPIOC

#define QSPI_DO3_Pin GPIO_PIN_1
#define QSPI_DO3_GPIO_Port GPIOA

#define QSPI_CSN_Pin GPIO_PIN_6
#define QSPI_CSN_GPIO_Port GPIOB

#define QSPI_SCk_Pin GPIO_PIN_1
#define QSPI_SCk_GPIO_Port GPIOB

#define OLED_TE_Pin GPIO_PIN_4
#define OLED_TE_GPIO_Port GPIOB
#define OLED_TE_EXTI_IRQn EXTI4_IRQn

#define OLED_RST_Pin GPIO_PIN_5
#define OLED_RST_GPIO_Port GPIOB

/*****************************************KEY**********************************************/
#define KEY_S_Pin GPIO_PIN_3
#define KEY_S_GPIO_Port GPIOB

#define KEY_U_Pin GPIO_PIN_2
#define KEY_U_GPIO_Port GPIOD

#define KEY_D_Pin GPIO_PIN_12
#define KEY_D_GPIO_Port GPIOC

#define KEY_L_Pin GPIO_PIN_11
#define KEY_L_GPIO_Port GPIOC

#define KEY_R_Pin GPIO_PIN_15
#define KEY_R_GPIO_Port GPIOA

#define MOVING_Pin GPIO_PIN_12
#define MOVING_GPIO_Port GPIOA
/*****************************************BLUE**********************************************/
#define EN_RANGE_Pin GPIO_PIN_0
#define EN_RANGE_GPIO_Port GPIOA
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA

#define LT8619_RST_Pin GPIO_PIN_0
#define LT8619_RST_GPIO_Port GPIOB
#define BOOT1_B1_EN_Pin GPIO_PIN_2
#define BOOT1_B1_EN_GPIO_Port GPIOB
#define SPI2_NSS_Pin GPIO_PIN_12
#define SPI2_NSS_GPIO_Port GPIOB
#define I2C3_SCL_Pin GPIO_PIN_6
#define I2C3_SCL_GPIO_Port GPIOC
#define I2C3_SDA_Pin GPIO_PIN_7
#define I2C3_SDA_GPIO_Port GPIOC
#define LT6911C_RST_N_Pin GPIO_PIN_10
#define LT6911C_RST_N_GPIO_Port GPIOA
#define T6911C_SLEEP_N_Pin GPIO_PIN_11
#define T6911C_SLEEP_N_GPIO_Port GPIOA

#define RST__Pin GPIO_PIN_15
#define RST__GPIO_Port GPIOA

#define Bl_RST_Pin GPIO_PIN_8
#define Bl_RST_GPIO_Port GPIOA

#define D_ON_Pin GPIO_PIN_0
#define D_ON_GPIO_Port GPIOA




#ifdef __cplusplus
}
#endif
#endif /* __MAIN_H */

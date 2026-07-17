/**
  ******************************************************************************
  * @file    Templates/Inc/main.h 
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
// #include "stm32f072b_discovery.h"   discovery开发板硬件配置

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Error_Handler(void);
uint8_t EEROM_Initial(void);

#define MCU_HPD_Pin GPIO_PIN_11
#define MCU_HPD_GPIO_Port GPIOB
#define EN_VDD_Pin GPIO_PIN_12
#define EN_VDD_GPIO_Port GPIOB
#define EN_ELVDD_Pin GPIO_PIN_13
#define EN_ELVDD_GPIO_Port GPIOB
#define EN_ELVSS_Pin GPIO_PIN_14
#define EN_ELVSS_GPIO_Port GPIOB
#define LT6911C_RES_Pin GPIO_PIN_15
#define LT6911C_RES_GPIO_Port GPIOB


#ifdef __cplusplus
}
#endif


#endif /* __MAIN_H */
/**
  ******************************************************************************
  * @file    UserDefine.h
  * @brief   This file contains all the function prototypes for
  *          the UserDefine.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 INT Technology.All rights reserved.
  *
  * This software component is licensed by INT under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  * opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USERDEFINE_H__
#define __USERDEFINE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* USER CODE BEGIN Includes */
#include <stdbool.h>
//#include <I2C.h>
//Eric_H:026414
#include <i2c.h>
#include "EEFlash.h"
/* USER CODE END Includes */


/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */


/* USER CODE BEGIN Prototypes */
bool PC_CMD_ChkSum(uint8_t *ptr);
uint8_t UnPackRead(uint8_t *ptr);
bool UnPackWrite(uint8_t *ptr);
/* USER CODE END Prototypes */


uint8_t * UnPackRead_ECX343EN(uint8_t *ptr);
bool UnPackWrite_EraseFlash_LT6911C(uint8_t *ptr);
bool UnPackWrite_ECX343EN_DSPON_SEQ(uint8_t *ptr);
bool UnPackWrite_ECX343EN_Power_On_SEQ(uint8_t *ptr);
bool UnPackWrite_Flash_LT6911C(uint8_t *ptr);
bool UnPackWrite_ECX343EN_BIST_SEQ(uint8_t *ptr);
bool UnPackWrite_2LT6911C(uint8_t *ptr);
bool UnPackRead_2I2Cdevice8N(uint8_t *ptr);
bool UnPackWrite_2I2Cdevice8N(uint8_t *ptr);
bool UnPackWrite_2EEPROM(uint8_t *ptr);
bool UnPackWrite_2I2Cdevice(uint8_t *ptr);
uint8_t UnPackRead_2I2Cdevice(uint8_t *ptr);
uint8_t UnPackRead_2EEPROM(uint8_t *ptr);
bool UnPackWrite_ECX343EN(uint8_t *ptr);
uint8_t UnPackRead_2LT6911C(uint8_t *ptr);

#ifdef __cplusplus
}
#endif

#endif /* __USERDEFINE_H__ */

/************************ (C) COPYRIGHT INT Tech *****END OF FILE****/

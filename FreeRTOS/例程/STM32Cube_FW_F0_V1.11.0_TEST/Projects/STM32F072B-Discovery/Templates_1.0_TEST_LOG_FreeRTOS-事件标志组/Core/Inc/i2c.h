/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c1;


#define Module_Write_L011_Addr	0x1E //
#define Module_Read_L011_Addr	  0x1F //

/* USER CODE BEGIN Private defines */
#define Module_Write_Addr	0x5C //
#define Module_Read_Addr	0x5D

#define EEROM_Write_Addr	0xA0 //
#define EEROM_Read_Addr		0xA0

//Eric_H:026411
#define LT6911C_Write_Addr	0x56 //
//0x2B, 0x48, 0xD2
#define LT6911C_Read_Addr	0x57

//Eric_H:026412
#define LT6911C_CHIPID	0x0516
#define CHIPID_REGH	0xA001
#define CHIPID_REGL	0xA000
#define I2C_EN_REG	0x80EE
#define I2C_ENABLE	0x1
#define I2C_DISABLE	0x0
#define I2C_WRITE	0x1
#define I2C_READ	0x0

/* USER CODE END Private defines */

void MX_I2C1_Init(void);

/* USER CODE BEGIN Prototypes */
bool Module_Write(uint16_t RegAdd, uint8_t CMD);
uint8_t Module_Read(uint16_t RegAdd);

uint8_t I2Cdevice_Read8(uint8_t I2Cdevice_Read_Addr, uint8_t RegAdd);
uint8_t I2Cdevice_Read(uint8_t I2Cdevice_Read_Addr, uint16_t RegAdd);
uint8_t * I2Cdevice_Read8N(uint8_t I2Cdevice_Read_Addr, uint8_t RegAdd, uint8_t count);
bool I2Cdevice_Write8(uint8_t I2Cdevice_Write_Addr, uint8_t RegAdd, uint8_t CMD);
uint8_t LT6911C_i2c_rd(uint8_t lt6911c_read_addr, uint16_t reg, uint8_t n);
bool I2Cdevice_Write8N(uint8_t I2Cdevice_Write_Addr, uint8_t RegAdd, uint8_t count, uint8_t *data);
bool I2Cdevice_Write(uint8_t I2Cdevice_Write_Addr, uint16_t RegAdd, uint8_t CMD);
bool LT6911C_i2c_wr(uint8_t lt6911c_write_addr, uint16_t reg, uint8_t n, uint8_t *values);
uint8_t * I2Cdevice_Read8N_PT(uint8_t I2Cdevice_Read_Addr, uint8_t RegAdd, uint8_t count, uint8_t *Receive);

//bool EEROM_Write(uint16_t RegAdd, uint8_t CMD);
//Eric_H:026414
//bool EEROM_Write(uint8_t RegAdd, uint8_t CMD);
//Eric_H:026414
#define EEROM_Write(RegAdd, CMD)	I2Cdevice_Write8(EEROM_Write_Addr, RegAdd, CMD)
//uint8_t EEROM_Read(uint16_t RegAdd);
//Eric_H:026414
#define EEROM_Read(RegAdd)	I2Cdevice_Read8(EEROM_Read_Addr, RegAdd)
/* USER CODE END Prototypes */

bool Module_Write_L011(uint16_t RegAdd, uint8_t CMD,uint8_t mode);
uint8_t Module_Read_L011(uint16_t RegAdd);
#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

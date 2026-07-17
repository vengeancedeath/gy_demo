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
#ifndef __EEFLASH_H__
#define __EEFLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define FLASH_STD_PAGE_SIZE       ((uint32_t)0x00000800)
#define EEFLASH_PAGE_SIZE         ((uint32_t)0x00000800)
#define EEFLASH_PAGE_ADDRESS      ((uint32_t)0x0801F800)
#define EEFLASH_PAGE_END_ADDRESS  (EEFLASH_PAGE_ADDRESS + EEFLASH_PAGE_SIZE)

#define EEFLASH_ITEM_MAX          30

typedef struct {
  uint16_t addr;
  uint16_t data;
} EEFlash_Item_t;

typedef struct {
  uint16_t total;  
  EEFlash_Item_t items[EEFLASH_ITEM_MAX];
} EEFlash_Table_t;

extern EEFlash_Table_t EEFlash_Table;


HAL_StatusTypeDef EEFlash_Erase(void);
HAL_StatusTypeDef EEFlash_Write(uint32_t offset, uint32_t data);
HAL_StatusTypeDef EEFlash_Read(uint32_t offset, uint32_t *data);
HAL_StatusTypeDef EEFlash_Table_Init(void);
HAL_StatusTypeDef EEFlash_Table_Save(void);
HAL_StatusTypeDef EEFlash_Table_Write(uint16_t addr, uint16_t data);
HAL_StatusTypeDef EEFlash_Table_Clear(void);
void EEFlash_Table_Print(void);

#ifdef __cplusplus
}
#endif

#endif /* __EEFLASH_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
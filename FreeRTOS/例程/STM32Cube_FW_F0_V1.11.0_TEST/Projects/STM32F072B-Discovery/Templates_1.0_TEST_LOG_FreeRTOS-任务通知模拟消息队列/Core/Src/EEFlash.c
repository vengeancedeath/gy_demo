/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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

/* Includes ------------------------------------------------------------------*/
#include "EEFlash.h"
#include "stdio.h"
#include "string.h"
#include "usbd_cdc_if.h"
#include "logger.h"

 const char *log_tag = "EEFlash";
#define EEFlash_INFO(format, ...) LOG_INFO(log_tag, format, ##__VA_ARGS__)
//#define printf	USB_Printf

static FLASH_EraseInitTypeDef EraseInitStruct;
static uint32_t PageError = 0;

EEFlash_Table_t EEFlash_Table;

/**
  * @brief  初始化EEFlash_Table
  *         从Flash读取total，若total为0或0xFFFF(未写入过)则清空整张表，
  *         若total不为0则从Flash读取所有数据项
  * @retval HAL_OK: 初始化成功, HAL_ERROR: 初始化失败
  */
HAL_StatusTypeDef EEFlash_Table_Init(void)
{
  uint32_t i;
  uint32_t word;

  if (EEFlash_Read(0, &word) != HAL_OK)
  {
    memset(&EEFlash_Table, 0, sizeof(EEFlash_Table_t));
    //打印
    EEFlash_INFO("EEFlash_Table_Init: read total error");
    return HAL_ERROR;
  }

  EEFlash_Table.total = (uint16_t)(word & 0xFFFF);

  if (EEFlash_Table.total == 0 || EEFlash_Table.total == 0xFFFF)
  {
    memset(&EEFlash_Table, 0, sizeof(EEFlash_Table_t));
    EEFlash_INFO("EEFlash_Table_Init: total=0, table cleared");
    return HAL_OK;
  }

  if (EEFlash_Table.total > EEFLASH_ITEM_MAX)
  {
    memset(&EEFlash_Table, 0, sizeof(EEFlash_Table_t));
    EEFlash_INFO("EEFlash_Table_Init: total=%d invalid, table cleared", EEFlash_Table.total);
    return HAL_ERROR;
  }

  for (i = 0; i < EEFlash_Table.total; i++)
  {
    if (EEFlash_Read((i + 1) * 4, &word) != HAL_OK)
    {
      memset(&EEFlash_Table, 0, sizeof(EEFlash_Table_t));
      //打印
      EEFlash_INFO("EEFlash_Table_Init: read item %d error", i);
      //清空整张表
      return HAL_ERROR;
    }
    EEFlash_Table.items[i].addr = (uint16_t)(word & 0xFFFF);
    EEFlash_Table.items[i].data = (uint16_t)((word >> 16) & 0xFFFF);
    EEFlash_INFO("EEFlash_Table_Init: item %d addr=0x%04X data=0x%04X", i, EEFlash_Table.items[i].addr, EEFlash_Table.items[i].data);
  }

  EEFlash_INFO("EEFlash_Table_Init: loaded %d items from Flash", EEFlash_Table.total);
  return HAL_OK;
}

/**
  * @brief  将EEFlash_Table保存到Flash
  *         先擦除整页，再依次写入total和所有数据项
  * @retval HAL_OK: 保存成功, HAL_ERROR: 保存失败
  */
HAL_StatusTypeDef EEFlash_Table_Save(void)
{
  uint32_t i;
  uint32_t word;

  if (EEFlash_Table.total > EEFLASH_ITEM_MAX)
  {
    EEFlash_INFO("EEFlash_Table_Save: total=%d invalid", EEFlash_Table.total);
    return HAL_ERROR;
  }

  if (EEFlash_Erase() != HAL_OK)
  {
    EEFlash_INFO("EEFlash_Table_Save: erase failed");
    return HAL_ERROR;
  }

  word = (uint32_t)EEFlash_Table.total;
  if (EEFlash_Write(0, word) != HAL_OK)
  {
    EEFlash_INFO("EEFlash_Table_Save: write total failed");
    return HAL_ERROR;
  }

  for (i = 0; i < EEFlash_Table.total; i++)
  {
    word = ((uint32_t)EEFlash_Table.items[i].data << 16) | EEFlash_Table.items[i].addr;
    if (EEFlash_Write((i + 1) * 4, word) != HAL_OK)
    {
      EEFlash_INFO("EEFlash_Table_Save: write item %d failed", i);
      return HAL_ERROR;
    }
    //打印
    EEFlash_INFO("EEFlash_Table_Save: item %d addr=0x%04X data=0x%04X", i, EEFlash_Table.items[i].addr, EEFlash_Table.items[i].data);
  }

  EEFlash_INFO("EEFlash_Table_Save: saved %d items to Flash", EEFlash_Table.total);
  return HAL_OK;
}

/**
  * @brief  向EEFlash_Table写入一条数据，若addr已存在则更新data，否则新增
  * @param  addr: 16位地址
  * @param  data: 16位数据
  * @retval HAL_OK: 写入成功, HAL_ERROR: 表已满或参数错误
  */
HAL_StatusTypeDef EEFlash_Table_Write(uint16_t addr, uint16_t data)
{
  uint32_t i;

  for (i = 0; i < EEFlash_Table.total; i++)
  {
    if (EEFlash_Table.items[i].addr == addr)
    {
      EEFlash_Table.items[i].data = data;
      EEFlash_INFO("EEFlash_Table_Write: update addr=0x%04X data=0x%04X", addr, data);
      return HAL_OK;
    }
  }

  if (EEFlash_Table.total >= EEFLASH_ITEM_MAX)
  {
    EEFlash_INFO("EEFlash_Table_Write: table full, total=%d", EEFlash_Table.total);
    return HAL_ERROR;
  }

  EEFlash_Table.items[EEFlash_Table.total].addr = addr;
  EEFlash_Table.items[EEFlash_Table.total].data = data;
  EEFlash_Table.total++;

  EEFlash_INFO("EEFlash_Table_Write: add addr=0x%04X data=0x%04X, total=%d", addr, data, EEFlash_Table.total);
  return HAL_OK;
}

/**
  * @brief  清空EEFlash_Table并擦除Flash
  * @retval HAL_OK: 清空成功, HAL_ERROR: Flash擦除失败
  */
HAL_StatusTypeDef EEFlash_Table_Clear(void)
{
  memset(&EEFlash_Table, 0, sizeof(EEFlash_Table_t));

  if (EEFlash_Erase() != HAL_OK)
  {
    EEFlash_INFO("EEFlash_Table_Clear: erase failed");
    return HAL_ERROR;
  }

  EEFlash_INFO("EEFlash_Table_Clear: table and Flash cleared");
  return HAL_OK;
}

/**
  * @brief  打印EEFlash_Table中所有数据项
  * @retval 无
  */
void EEFlash_Table_Print(void)
{
  uint32_t i;

  EEFlash_INFO("===== EEFlash_Table total=%d =====", EEFlash_Table.total);

  for (i = 0; i < EEFlash_Table.total; i++)
  {
    EEFlash_INFO("[%2d] addr=0x%04X  data=0x%04X", i, EEFlash_Table.items[i].addr, EEFlash_Table.items[i].data);
  }

  EEFlash_INFO("==================================");
}




HAL_StatusTypeDef EEFlash_Erase(void)
{
  // HAL_StatusTypeDef status;

  HAL_FLASH_Unlock();

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = EEFLASH_PAGE_ADDRESS;
  EraseInitStruct.NbPages     = (EEFLASH_PAGE_END_ADDRESS - EEFLASH_PAGE_ADDRESS) / FLASH_STD_PAGE_SIZE;

  if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
  {
    HAL_FLASH_Lock();
    //打印
    EEFlash_INFO("EEFlash_Erase Error: 0x%08X", PageError);
    
    return HAL_ERROR;
  }

  HAL_FLASH_Lock();
    //打印
  EEFlash_INFO("EEFlash_Erase success");

  return HAL_OK;
}

/**
  * @brief  向Flash最后一页写入一个32位数据
  * @param  offset: 页内偏移地址，必须4字节对齐(offset % 4 == 0)
  * @param  data:   要写入的32位数据
  * @retval HAL_OK: 写入成功, HAL_ERROR: 参数错误或写入失败
  */
HAL_StatusTypeDef EEFlash_Write(uint32_t offset, uint32_t data)
{
  uint32_t address;

  if ((offset % 4) != 0)
  {
    //打印
    EEFlash_INFO("EEFlash_Write Error: offset %d is not 4 bytes aligned", offset);
    return HAL_ERROR;
  }

  address = EEFLASH_PAGE_ADDRESS + offset;

  if (address >= EEFLASH_PAGE_END_ADDRESS)
  {
    //打印
    EEFlash_INFO("EEFlash_Write Error: address 0x%08X is out of range", address);
    return HAL_ERROR;
  }

  HAL_FLASH_Unlock();

  if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data) != HAL_OK)
  {
    HAL_FLASH_Lock();
    //打印
    EEFlash_INFO("EEFlash_Write Error at address 0x%08X", address);
    return HAL_ERROR;
  }

    //打印
  EEFlash_INFO("EEFlash_Write success");
  HAL_FLASH_Lock();

  return HAL_OK;
}

HAL_StatusTypeDef EEFlash_Read(uint32_t offset, uint32_t *data)
{
  uint32_t address;

  if ((offset % 4) != 0)
  {
    //打印
    EEFlash_INFO("EEFlash_Read Error: offset %d is not 4 bytes aligned", offset);
    return HAL_ERROR;
  }

  address = EEFLASH_PAGE_ADDRESS + offset;

  if (address >= EEFLASH_PAGE_END_ADDRESS)
  {
    //打印
    EEFlash_INFO("EEFlash_Read Error: address %d is out of range", address);
    return HAL_ERROR;
  }
  
  *data = *(__IO uint32_t *)address;
  
  return HAL_OK;
}
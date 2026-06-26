/**
  ******************************************************************************
  * @file    optionbytes_interface.c
  * @author  MCD Application Team
  * @brief   Contains Option Bytes access functions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "openbl_mem.h"
#include "app_openbootloader.h"
#include "common_interface.h"
#include "optionbytes_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef OB_Descriptor =
{
  OB_START_ADDRESS,
  OB_END_ADDRESS,
  (16),
  OB_AREA,
  OPENBL_OB_Read,
  OPENBL_OB_Write,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};


/* Exported functions --------------------------------------------------------*/

/**
  * @brief  读取选项字节的指定地址内容
  *         选项字节映射在系统存储区（F072: 0x1FFFF800），可直接通过内存地址读取
  * @param  Address 选项字节地址（应在 OB_START_ADDRESS ~ OB_END_ADDRESS 范围内）
  * @retval 读取到的字节值
  */
uint8_t OPENBL_OB_Read(uint32_t Address)
{
  return (*(volatile uint8_t *)(Address));
}

/**
  * @brief  写入选项字节
  *
  *         STM32F072 选项字节的物理布局（起始地址 0x1FFFF800）：
  *         偏移 0x00: RDP   (16-bit) - 读保护等级（0xAA=Level0, 0xBB=Level1, 0xCC=Level2）
  *         偏移 0x02: USER  (16-bit) - 用户配置（IWDG/STOP/STDBY/BOOT1/VDDA/SRAM_PARITY）
  *         偏移 0x04: DATA0 (16-bit) - 用户数据字节0
  *         偏移 0x06: DATA1 (16-bit) - 用户数据字节1
  *         偏移 0x08: WRP0  (16-bit) - 写保护0
  *         偏移 0x0A: WRP1  (16-bit) - 写保护1
  *         偏移 0x0C: WRP2  (16-bit) - 写保护2
  *         偏移 0x0E: WRP3  (16-bit) - 写保护3
  *         总计 16 字节（0x1FFFF800 ~ 0x1FFFF80F）
  *
  *         数据格式（Data 缓冲区）：
  *         Data[0]      : RDP 等级值
  *         Data[1]      : USER 配置值
  *         Data[2]      : DATA0 值
  *         Data[3]      : DATA1 值
  *         Data[4..7]   : WRP0~WRP3 写保护值
  *
  * @param  Address    选项字节目标地址（未使用，保留兼容接口）
  * @param  Data       数据缓冲区指针
  * @param  DataLength 数据长度（字节数）
  * @retval 无
  */
void OPENBL_OB_Write(uint32_t Address, uint8_t *Data, uint32_t DataLength)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  HAL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();

  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

  /* 写入 RDP 等级 */
  if (DataLength >= 1)
  {
    flash_ob.OptionType = OPTIONBYTE_RDP;
    flash_ob.RDPLevel   = Data[0];
    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  /* 写入 USER 配置 */
  if (DataLength >= 2)
  {
    flash_ob.OptionType = OPTIONBYTE_USER;
    flash_ob.USERConfig = Data[1];
    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  /* 写入 DATA0 */
  if (DataLength >= 3)
  {
    flash_ob.OptionType   = OPTIONBYTE_DATA;
    flash_ob.DATAAddress  = OB_DATA_ADDRESS_DATA0;
    flash_ob.DATAData     = Data[2];
    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  /* 写入 DATA1 */
  if (DataLength >= 4)
  {
    flash_ob.OptionType   = OPTIONBYTE_DATA;
    flash_ob.DATAAddress  = OB_DATA_ADDRESS_DATA1;
    flash_ob.DATAData     = Data[3];
    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  /* 写入写保护（WRP0~WRP3 组合成 WRPPage 掩码） */
  if (DataLength >= 8)
  {
    flash_ob.OptionType = OPTIONBYTE_WRP;
    flash_ob.WRPState   = OB_WRPSTATE_ENABLE;
    flash_ob.WRPPage    = (uint32_t)(Data[4]) | ((uint32_t)(Data[5]) << 8) |
                          ((uint32_t)(Data[6]) << 16) | ((uint32_t)(Data[7]) << 24);
    HAL_FLASHEx_OBProgram(&flash_ob);
  }

//  HAL_FLASH_OB_Launch();

//  HAL_FLASH_OB_Lock();
//  HAL_FLASH_Lock();
}
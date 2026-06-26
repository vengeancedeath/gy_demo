/**
  ******************************************************************************
  * @file    openbl_mem.h
  * @author  MCD Application Team
  * @brief   Header for openbl_mem.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#ifndef OPENBL_MEM_H
#define OPENBL_MEM_H

/* Includes ------------------------------------------------------------------*/
#include "openbootloader_conf.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint32_t StartAddress; // 内存起始地址
  uint32_t EndAddress; // 内存结束地址
  uint32_t Size; // 内存大小
  uint32_t Type; // 内存类型
  uint8_t (*Read)(uint32_t Address); // 读取函数
  void (*Write)(uint32_t Address, uint8_t *Data, uint32_t DataLength); // 写入函数
  void (*SetReadoutProtect)(uint32_t State); // 设置读取保护函数
  ErrorStatus(*SetWriteProtect)(FunctionalState State, uint8_t *Buffer, uint32_t Length); // 设置写入保护函数
  void (*JumpToAddress)(uint32_t Address); // 跳转函数
  ErrorStatus(*MassErase)(uint8_t *p_Data, uint32_t DataLength); // 批量擦除函数
  ErrorStatus(*Erase)(uint8_t *p_Data, uint32_t DataLength); // 擦除函数
} OPENBL_MemoryTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void OPENBL_MEM_JumpToAddress(uint32_t Address);
void OPENBL_MEM_OptionBytesLaunch(void);
void OPENBL_MEM_SetReadOutProtection(FunctionalState State);
void OPENBL_MEM_Write(uint32_t Address, uint8_t *Data, uint32_t DataLength);

uint8_t OPENBL_MEM_Read(uint32_t Address, uint32_t MemoryIndex);
uint32_t OPENBL_MEM_GetAddressArea(uint32_t Address);
uint32_t OPENBL_MEM_GetMemoryIndex(uint32_t Address);

FlagStatus OPENBL_MEM_GetReadOutProtectionStatus(void);

ErrorStatus OPENBL_MEM_Erase(uint32_t Address, uint8_t *p_Data, uint32_t DataLength);
ErrorStatus OPENBL_MEM_MassErase(uint32_t Address, uint8_t *p_Data, uint32_t DataLength);
ErrorStatus OPENBL_MEM_RegisterMemory(OPENBL_MemoryTypeDef *Memory);
ErrorStatus OPENBL_MEM_SetWriteProtection(FunctionalState State, uint32_t Address, uint8_t *Buffer, uint32_t Length);

#endif /* OPENBL_MEM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

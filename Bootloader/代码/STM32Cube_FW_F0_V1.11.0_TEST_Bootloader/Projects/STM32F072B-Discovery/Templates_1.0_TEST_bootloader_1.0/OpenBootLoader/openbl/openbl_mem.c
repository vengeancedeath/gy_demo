/**
  ******************************************************************************
  * @file    openbl_mem.c
  * @author  MCD Application Team
  * @brief   Provides functions that manage operations on embedded memories (Flash, SRAM...).
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

/* Includes ------------------------------------------------------------------*/
#include "openbl_mem.h"
#include "openbl_core.h"

#include "flash_interface.h"
#include "interfaces_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t NumberOfMemories = 0;
static OPENBL_MemoryTypeDef a_MemoriesTable[MEMORIES_SUPPORTED];

/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used to register memory interfaces in Open Bootloader MW.
  * @param  *Memory A pointer to the memory handle.
  * @return ErrorStatus Returns ERROR in case of no more space in the memories table else returns SUCCESS.
  */
ErrorStatus OPENBL_MEM_RegisterMemory(OPENBL_MemoryTypeDef *Memory)
{
  ErrorStatus status = SUCCESS;

  if (NumberOfMemories < MEMORIES_SUPPORTED)
  {
    a_MemoriesTable[NumberOfMemories].StartAddress      = Memory->StartAddress;
    a_MemoriesTable[NumberOfMemories].EndAddress        = Memory->EndAddress;
    a_MemoriesTable[NumberOfMemories].Size              = Memory->Size;
    a_MemoriesTable[NumberOfMemories].Type              = Memory->Type;
    a_MemoriesTable[NumberOfMemories].Read              = Memory->Read;
    a_MemoriesTable[NumberOfMemories].Write             = Memory->Write;
    a_MemoriesTable[NumberOfMemories].SetReadoutProtect = Memory->SetReadoutProtect;
    a_MemoriesTable[NumberOfMemories].SetWriteProtect   = Memory->SetWriteProtect;
    a_MemoriesTable[NumberOfMemories].JumpToAddress     = Memory->JumpToAddress;
    a_MemoriesTable[NumberOfMemories].MassErase         = Memory->MassErase;
    a_MemoriesTable[NumberOfMemories].Erase             = Memory->Erase;

    NumberOfMemories++;
  }
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  判断给定地址所属的存储器区域类型。
  *
  *         遍历已注册的存储器描述符表，检查地址是否落在某个存储器的
  *         [StartAddress, EndAddress) 范围内，匹配则返回该存储器的 Type 字段。
  *
  *         返回值用于命令层做地址合法性校验和功能限制判断，例如：
  *           - OPENBL_USART_GetAddress()：AREA_ERROR → 地址无效，回复 NACK
  *           - OPENBL_USART_Go()：仅允许 FLASH_AREA 和 RAM_AREA 执行跳转
  *           - OPENBL_USART_WriteMemory()：OB_AREA 写入后需触发 OBL 重载
  *
  *         可能的返回值：
  *           FLASH_AREA  (0x00) - 主 Flash
  *           RAM_AREA    (0x01) - SRAM
  *           OB_AREA     (0x02) - 选项字节
  *           OTP_AREA    (0x03) - 一次性可编程区
  *           ICP1_AREA   (0x04) - 系统存储器 Bank1
  *           ICP2_AREA   (0x05) - 系统存储器 Bank2
  *           EB_AREA     (0x06) - 工程字节
  *           AREA_ERROR  (0x07) - 地址不在任何已注册存储器范围内
  *
  * @param  Address 待检查的 32 位目标地址
  * @retval 存储器区域类型（FLASH_AREA/RAM_AREA/OB_AREA 等），
  *         地址无效时返回 AREA_ERROR
  */
uint32_t OPENBL_MEM_GetAddressArea(uint32_t Address)
{
  uint32_t mem_area = AREA_ERROR;
  uint32_t counter;

  /* 遍历所有已注册的存储器描述符，逐个检查地址归属 */
  for (counter = 0; counter < NumberOfMemories; counter++)
  {
    /* 地址范围判断：左闭右开 [StartAddress, EndAddress) */
    if ((Address >= a_MemoriesTable[counter].StartAddress) && (Address < a_MemoriesTable[counter].EndAddress))
    {
      /* 匹配成功：取出该存储器的类型标识（FLASH_AREA/RAM_AREA 等） */
      mem_area = a_MemoriesTable[counter].Type;
      break;
    }
    else
    {
      /* 当前存储器不匹配，保持 AREA_ERROR，继续检查下一个 */
      mem_area = AREA_ERROR;
    }
  }

  return mem_area;
}

/**
  * @brief  根据地址查找所属存储器在描述符表中的索引。
  *
  *         遍历已注册的存储器描述符表 a_MemoriesTable，找到目标地址
  *         落在哪个存储器的 [StartAddress, EndAddress) 范围内，
  *         返回该存储器在表中的下标，用于后续通过下标直接访问其回调函数。
  *
  *         例如当前描述符注册顺序为：
  *           索引0 → FLASH (0x08000000~0x08080000)
  *           索引1 → RAM   (0x2000xxxx~0x20020000)
  *           索引2 → OB    (0x1FFF7800~0x1FFF7880)
  *           ...
  *         传入 0x08001000 将返回 0（FLASH），传入 0x1FFF7800 将返回 2（OB）。
  *
  * @note   若地址不在任何已注册存储器范围内，返回值等于 NumberOfMemories，
  *         调用方应检查返回值 < NumberOfMemories 以确认索引有效，
  *         否则用该索引访问 a_MemoriesTable 将越界。
  *
  * @param  Address 待查找的 32 位目标地址
  * @return 存储器在描述符表中的索引（0 ~ NumberOfMemories-1），
  *         未匹配时返回 NumberOfMemories（无效索引）
  */
uint32_t OPENBL_MEM_GetMemoryIndex(uint32_t Address)
{
  uint32_t counter;

  /* 遍历所有已注册的存储器描述符，逐个检查地址是否落在其范围内 */
  for (counter = 0; counter < NumberOfMemories; counter++)
  {
    /* 地址范围判断：左闭右开 [StartAddress, EndAddress) */
    if ((Address >= a_MemoriesTable[counter].StartAddress) && (Address < a_MemoriesTable[counter].EndAddress))
    {
      break;
    }
  }

  /* 匹配成功：counter 为目标存储器索引
   * 未匹配：counter == NumberOfMemories（无效索引，调用方需判断） */
  return counter;
}

/**
  * @brief  从指定存储器的指定地址读取 1 字节数据。
  *
  *         通过存储器索引直接定位描述符，调用其 Read 回调函数完成底层读取。
  *         典型调用链：OPENBL_USART_ReadMemory → 本函数 → FLASH/RAM/OTP 等的 Read 回调。
  *
  *         防御性处理：
  *           - 索引越界（>= NumberOfMemories）：返回 0，避免数组越界访问
  *           - Read 回调为 NULL（如 ICP/EB 等只读存储器未注册 Read）：返回 0
  *
  * @param  Address     待读取的 32 位目标地址
  * @param  MemoryIndex 存储器在描述符表中的索引，由 OPENBL_MEM_GetMemoryIndex() 获取
  * @return 读取到的 1 字节数据；索引越界或 Read 回调为 NULL 时返回 0
  */
uint8_t OPENBL_MEM_Read(uint32_t Address, uint32_t MemoryIndex)
{
  uint8_t value;

  /* ── 索引有效性检查 ──
   * 防止 GetMemoryIndex 未匹配时返回 NumberOfMemories 导致越界 */
  if (MemoryIndex < NumberOfMemories)
  {
    /* ── Read 回调非空检查 ──
     * 部分存储器（如 ICP1/ICP2/EB）虽注册了描述符但 Read 回调为 NULL，
     * 此时不调用回调，直接返回 0 */
    if (a_MemoriesTable[MemoryIndex].Read != NULL)
    {
      value = a_MemoriesTable[MemoryIndex].Read(Address);
    }
    else
    {
      value = 0;
    }
  }
  else
  {
    /* 索引越界，返回默认值 0 */
    value = 0;
  }

  return value;
}

/**
  * @brief  将数据写入指定地址所在的存储器。
  *
  *         根据地址查表定位目标存储器描述符，调用其 Write 回调完成底层写入。
  *         典型调用链：OPENBL_USART_WriteMemory → 本函数 → FLASH/RAM/OTP/OB 的 Write 回调。
  *
  *         不同存储器的 Write 回调行为差异：
  *           - FLASH：解锁 Flash → 双字编程 → 加锁（每次最多 256 字节）
  *           - RAM：直接内存拷贝（memcpy 等效）
  *           - OTP：双字编程，每位只能从 1 写为 0，不可逆
  *           - OB：写入选项字节寄存器，需后续调用 OptionBytesLaunch 触发 OBL 重载
  *           - ICP1/ICP2/EB：Write 回调为 NULL，静默忽略（只读存储器）
  *
  * @note   本函数不做读保护检查，该检查由命令层（如 OPENBL_USART_WriteMemory）
  *         在调用本函数之前完成。索引越界或 Write 回调为 NULL 时静默返回，
  *        不报错也不写入。
  *
  * @param  Address     目标写入起始地址（32 位）
  * @param  Data        指向待写入数据的缓冲区指针
  * @param  DataLength  待写入数据长度（字节数，1~256）
  * @retval None
  */
void OPENBL_MEM_Write(uint32_t Address, uint8_t *Data, uint32_t DataLength)
{
  uint32_t index;

  /* 根据地址查表获取存储器索引，确定数据应写入哪个存储器 */
  index = OPENBL_MEM_GetMemoryIndex(Address);

  /* ── 索引有效性检查 ──
   * 防止 GetMemoryIndex 未匹配时返回 NumberOfMemories 导致越界 */
  if (index < NumberOfMemories)
  {
    /* ── Write 回调非空检查 ──
     * 部分存储器（如 ICP1/ICP2/EB）为只读，Write 回调为 NULL，
     * 此时不调用回调，静默忽略写入请求 */
    if (a_MemoriesTable[index].Write != NULL)
    {
      a_MemoriesTable[index].Write(Address, Data, DataLength);
    }
  }
}

/**
  * @brief  Enables or disables the read out protection.
  * @param  State The readout protection state that will be set.
  * @retval None.
  */
void OPENBL_MEM_SetReadOutProtection(FunctionalState State)
{
  if (State == ENABLE)
  {
    OPENBL_FLASH_SetReadOutProtectionLevel(RDP_LEVEL_1);
  }
  else
  {
    OPENBL_FLASH_SetReadOutProtectionLevel(RDP_LEVEL_0);
  }
}

/**
  * @brief  查询 Flash 读保护（Read Out Protection）的当前状态。
  *
  *         通过底层 FLASH 接口读取当前 RDP 等级，转换为 SET/RESET 语义：
  *           - RDP_LEVEL_0（无保护）→ RESET：读保护未使能，允许 ReadMemory 等操作
  *           - RDP_LEVEL_1（读保护）→ SET：读保护已使能，拒绝读取 Flash 内容
  *           - RDP_LEVEL_2（永久读保护）→ SET：读保护已使能且不可逆
  *
  *         此函数被所有需要检查读保护状态的命令（ReadMemory、WriteMemory、
  *         Go、EraseMemory、WriteProtect、WriteUnprotect）在入口处调用，
  *         若返回 SET 则直接回复 NACK 拒绝操作。
  *
  * @note   Level 1 和 Level 2 在此函数中不做区分，均返回 SET。
  *         Level 2 的不可逆特性在 OPENBL_FLASH_SetReadOutProtectionLevel() 中处理。
  *
  * @return SET  读保护已使能（RDP Level 1 或 Level 2）
  *         RESET 读保护未使能（RDP Level 0）
  */
FlagStatus OPENBL_MEM_GetReadOutProtectionStatus(void)
{
  FlagStatus status;

  /* 读取当前 RDP 等级：Level 0 = 无保护，Level 1/2 = 读保护已使能 */
  if (OPENBL_FLASH_GetReadOutProtectionLevel() != RDP_LEVEL_0)
  {
    /* RDP Level 1 或 Level 2：读保护生效 */
    status = SET;
  }
  else
  {
    /* RDP Level 0：无读保护，允许正常访问 */
    status = RESET;
  }

  return status;
}

/**
  * @brief  This function is used to enable or disable write protection of the specified FLASH areas.
  * @param  State Can be one of these values:
  *         @arg DISABLE: Disable FLASH write protection
  *         @arg ENABLE: Enable FLASH write protection
  * @param  Address Contains the address of the memory where write protection state will be changed
  * @param  Buffer Contains write protection operation options
  * @param  Length The length of the write protection options Buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
ErrorStatus OPENBL_MEM_SetWriteProtection(FunctionalState State, uint32_t Address, uint8_t *Buffer, uint32_t Length)
{
  uint32_t index;
  ErrorStatus status = SUCCESS;

  /* Get the memory index to know in which memory we will write */
  index = OPENBL_MEM_GetMemoryIndex(Address);

  if (index < NumberOfMemories)
  {
    if (a_MemoriesTable[index].SetWriteProtect != NULL)
    {
      a_MemoriesTable[index].SetWriteProtect(State, Buffer, Length);
    }
    else
    {
      status = ERROR;
    }
  }
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  This function is used to jump to an address of an application.
  * @param  Address User application address.
  * @retval None.
  */
void OPENBL_MEM_JumpToAddress(uint32_t Address)
{
  OPENBL_FLASH_JumpToAddress(Address);
}

/**
  * @brief  This function is used to start a mass erase operation on a given memory.
  * @param  Address The address of the memory to be fully erased.
  * @param  *p_Data Pointer to the buffer that contains mass erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Mass erase operation done
  *          - ERROR:   Mass erase operation failed or one parameter is invalid
 */
ErrorStatus OPENBL_MEM_MassErase(uint32_t Address, uint8_t *p_Data, uint32_t DataLength)
{
  uint32_t memory_index;
  ErrorStatus status;

  /* Get the memory index to know from which memory interface we will used */
  memory_index = OPENBL_MEM_GetMemoryIndex(Address);

  if (memory_index < NumberOfMemories)
  {
    if (a_MemoriesTable[memory_index].MassErase != NULL)
    {
      status = a_MemoriesTable[memory_index].MassErase(p_Data, DataLength);
    }
    else
    {
      status = ERROR;
    }
  }
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  This function is used to erase the specified memory.
  * @param  Address The address of the memory to be erased.
  * @param  *p_Data Pointer to the buffer that contains erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Erase operation done
  *          - ERROR:   Erase operation failed or one parameter is invalid
 */
ErrorStatus OPENBL_MEM_Erase(uint32_t Address, uint8_t *p_Data, uint32_t DataLength)
{
  uint32_t memory_index;
  ErrorStatus status;

  /* Get the memory index to know from which memory interface we will used */
  memory_index = OPENBL_MEM_GetMemoryIndex(Address);

  if (memory_index < NumberOfMemories)
  {
    if (a_MemoriesTable[memory_index].Erase != NULL)
    {
      status = a_MemoriesTable[memory_index].Erase(p_Data, DataLength);
    }
    else
    {
      status = ERROR;
    }
  }
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  Launch the option byte loading.
  * @retval None.
  */
void OPENBL_MEM_OptionBytesLaunch(void)
{
  OPENBL_FLASH_OB_Launch();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
/**
  ******************************************************************************
  * @file    openbl_usart_cmd.c
  * @author  MCD Application Team
  * @brief   Contains USART protocol commands
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
#include "openbl_usart_cmd.h"

#include "openbootloader_conf.h"
#include "usart_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define OPENBL_USART_COMMANDS_NB          11U       /* Number of supported commands */

#define USART_RAM_BUFFER_SIZE             1156U     /* Size of USART buffer used to store received data from the host */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Buffer used to store received data from the host */
static uint8_t USART_RAM_Buf[USART_RAM_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void OPENBL_USART_GetCommand(void);
static void OPENBL_USART_GetVersion(void);
static void OPENBL_USART_GetID(void);
static void OPENBL_USART_ReadMemory(void);
static void OPENBL_USART_WriteMemory(void);
static void OPENBL_USART_Go(void);
static void OPENBL_USART_ReadoutProtect(void);
static void OPENBL_USART_ReadoutUnprotect(void);
static void OPENBL_USART_EraseMemory(void);
static void OPENBL_USART_WriteProtect(void);
static void OPENBL_USART_WriteUnprotect(void);


static uint8_t OPENBL_USART_GetAddress(uint32_t *Address);


/* Exported variables --------------------------------------------------------*/
OPENBL_CommandsTypeDef OPENBL_USART_Commands =
{
  OPENBL_USART_GetCommand,
  OPENBL_USART_GetVersion,
  OPENBL_USART_GetID,
  OPENBL_USART_ReadMemory,
  OPENBL_USART_WriteMemory,
  OPENBL_USART_Go,
  OPENBL_USART_ReadoutProtect,
  OPENBL_USART_ReadoutUnprotect,
  OPENBL_USART_EraseMemory,
  OPENBL_USART_WriteProtect,
  OPENBL_USART_WriteUnprotect,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Exported functions---------------------------------------------------------*/
/**
  * @brief  This function is used to get a pointer to the structure that contains the available USART commands.
  * @return Returns a pointer to the OPENBL_USART_Commands struct.
  */
OPENBL_CommandsTypeDef *OPENBL_USART_GetCommandsList(void)
{
  return (&OPENBL_USART_Commands);
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  处理 Get 命令（命令码 0x00），向主机返回本 Bootloader 支持的命令列表。
  *
  *         遵循 ST AN3155 USART 协议规范，响应帧格式为：
  *           ACK → 命令数量(N) → 协议版本 → 命令码[0]…命令码[N-1] → ACK
  *
  *         主机通过此命令探测设备能力：支持哪些命令、协议版本号是多少。
  *         这是握手成功后主机通常发送的第一个命令。
  *
  * @note   响应帧中 "命令数量" 字段的值 = 实际支持的命令个数，
  *         主机据此确定后续需要读取多少个命令码字节。
  *         当前共支持 11 个命令（OPENBL_USART_COMMANDS_NB = 11）。
  *
  * @retval None
  */
static void OPENBL_USART_GetCommand(void)
{
  uint32_t counter;

  /* 本 Bootloader 支持的全部命令码列表，顺序无关但需与 OPENBL_USART_Commands
     函数指针表一一对应，数量由 OPENBL_USART_COMMANDS_NB 宏约束 */
  const uint8_t a_OPENBL_USART_CommandsList[OPENBL_USART_COMMANDS_NB] =
  {
    CMD_GET_COMMAND,       /* 0x00 - Get：获取支持的命令列表和协议版本             */
    CMD_GET_VERSION,       /* 0x01 - Get Version：获取 Bootloader 协议版本号       */
    CMD_GET_ID,            /* 0x02 - Get ID：获取芯片设备 ID                        */
    CMD_READ_MEMORY,       /* 0x11 - Read Memory：从指定地址读取内存数据            */
    CMD_GO,                /* 0x21 - Go：跳转到指定地址执行用户应用                 */
    CMD_WRITE_MEMORY,      /* 0x31 - Write Memory：向指定地址写入数据               */
    CMD_EXT_ERASE_MEMORY,  /* 0x44 - Extended Erase Memory：擦除指定 Flash 页       */
    CMD_WRITE_PROTECT,     /* 0x63 - Write Protect：使能 Flash 写保护               */
    CMD_WRITE_UNPROTECT,   /* 0x73 - Write Unprotect：禁用 Flash 写保护             */
    CMD_READ_PROTECT,      /* 0x82 - Readout Protect：使能读保护 (RDP Level 1)      */
    CMD_READ_UNPROTECT     /* 0x92 - Readout Unprotect：禁用读保护 (RDP Level 0)    */
  };

  /* ── 响应帧第1字节：ACK (0x79)，通知主机命令已被识别 ── */
  OPENBL_USART_SendByte(ACK_BYTE);

  /* ── 响应帧第2字节：支持的命令数量 N ──
   * 主机据此计算后续命令码字节数（AN3155 规范中此字段含义为
   * "Number of bytes to follow - 1"，ST OpenBL 实现直接发送命令个数，
   * 主机端 CubeProgrammer 已适配此格式） */
  OPENBL_USART_SendByte(OPENBL_USART_COMMANDS_NB);

  /* ── 响应帧第3字节：协议版本号（当前 0x31 = V3.1） ── */
  OPENBL_USART_SendByte(OPENBL_USART_VERSION);

  /* ── 响应帧第4~N+3字节：逐个发送支持的命令码 ── */
  for (counter = 0U; counter < OPENBL_USART_COMMANDS_NB; counter++)
  {
    OPENBL_USART_SendByte(a_OPENBL_USART_CommandsList[counter]);
  }

  /* ── 响应帧最后一字节：ACK (0x79)，帧结束同步标记 ── */
  OPENBL_USART_SendByte(ACK_BYTE);
}

/**
  * @brief  处理 Get Version 命令（命令码 0x01），向主机返回 Bootloader 协议版本号。
  *
  *         遵循 ST AN3155 USART 协议规范，响应帧格式为：
  *           ACK → 版本号 → 0x00 → 0x00 → ACK
  *
  *         其中两个 0x00 字节为 AN3155 协议规定的保留字段，
  *         主机端 CubeProgrammer 需要按此固定长度读取完整响应帧。
  *
  * @note   当前协议版本为 0x31（即 V3.1），由 OPENBL_USART_VERSION 宏定义。
  *
  * @retval None
  */
static void OPENBL_USART_GetVersion(void)
{
  /* ── 响应帧第1字节：ACK (0x79)，通知主机命令已被识别 ── */
  OPENBL_USART_SendByte(ACK_BYTE);

  /* ── 响应帧第2字节：协议版本号（0x31 = V3.1） ── */
  OPENBL_USART_SendByte(OPENBL_USART_VERSION);

  /* ── 响应帧第3~4字节：协议保留字段，固定填 0x00 ──
   * AN3155 规范要求响应帧包含这两个字节，主机按固定长度解析，
   * 实际无有效信息，仅用于帧长度对齐 */
  OPENBL_USART_SendByte(0x00);
  OPENBL_USART_SendByte(0x00);

  /* ── 响应帧最后一字节：ACK (0x79)，帧结束同步标记 ── */
  OPENBL_USART_SendByte(ACK_BYTE);
}

/**
  * @brief  处理 Get ID 命令（命令码 0x02），向主机返回芯片设备 ID。
  *
  *         遵循 ST AN3155 USART 协议规范，响应帧格式为：
  *           ACK → N → ID_MSB → ID_LSB → ACK
  *
  *         其中 N=0x01 表示后续设备 ID 占 2 字节（N+1=2），
  *         设备 ID 为 0x0447，对应 STM32F072xx 系列。
  *
  * @note   设备 ID 定义在 openbootloader_conf.h 中：
  *         DEVICE_ID_MSB = 0x04, DEVICE_ID_LSB = 0x47。
  *         主机端 CubeProgrammer 据此识别目标芯片型号。
  *
  * @retval None
  */
static void OPENBL_USART_GetID(void)
{
  /* ── 响应帧第1字节：ACK (0x79)，通知主机命令已被识别 ── */
  OPENBL_USART_SendByte(ACK_BYTE);

  /* ── 响应帧第2字节：N = 0x01 ──
   * AN3155 协议规定 N 表示后续 ID 数据字节数减 1，
   * 即 N+1 = 2 字节设备 ID */
  OPENBL_USART_SendByte(0x01);

  /* ── 响应帧第3~4字节：设备 ID（高字节在前） ──
   * 0x0447 = STM32F072xx，主机据此匹配对应的 Flash 布局和算法 */
  OPENBL_USART_SendByte(DEVICE_ID_MSB);
  OPENBL_USART_SendByte(DEVICE_ID_LSB);

  /* ── 响应帧最后一字节：ACK (0x79)，帧结束同步标记 ── */
  OPENBL_USART_SendByte(ACK_BYTE);
}

/**
 * @brief  处理 Read Memory 命令（命令码 0x11），从指定地址读取内存数据并发送给主机。
 *
 *         遵循 ST AN3155 USART 协议规范，交互流程为：
 *           1. MCU 收到命令码后，先检查读保护状态 ，回复 ACK 或 NACK
 *           2. 主机发送起始地址（4 字节 + XOR 校验），MCU 校验后回复 ACK
 *           3. 主机发送读取长度 N 及其补码 N^0xFF，MCU 校验后回复 ACK
 *           4. MCU 从目标地址逐字节读取 (N+1) 个数据并发送给主机
 *
 *         读取长度编码：N 的范围 0x00~0xFF，实际读取字节数 = N + 1（1~256 字节）
 *
 * @note   若读保护已使能（RDP Level != 0），直接回复 NACK 拒绝访问。
 *         地址校验失败或长度校验失败同样回复 NACK。
 *
 * @retval None
 */
static void OPENBL_USART_ReadMemory(void)
{
  uint32_t address;
  uint32_t counter;
  uint32_t memory_index;
  uint8_t data;
  uint8_t xor;

  /* ── 第1步：读保护检查 ──
   * 若 RDP Level != 0（读保护已使能），拒绝读取并回复 NACK */
  if (OPENBL_MEM_GetReadOutProtectionStatus() != RESET)
  {
    OPENBL_USART_SendByte(NACK_BYTE);
  }
  else
  {
    /* ── 第2步：接收并校验目标地址 ──
     * 主机发送：addr[31:24] + addr[23:16] + addr[15:8] + addr[7:0] + XOR
     * GetAddress 内部校验 XOR 和地址合法性，返回 NACK 表示校验失败 */
    OPENBL_USART_SendByte(ACK_BYTE);

    if (OPENBL_USART_GetAddress(&address) == NACK_BYTE)
    {
      OPENBL_USART_SendByte(NACK_BYTE);
    }
    else
    {
      /* ── 第3步：接收并校验读取长度 ──
       * 主机发送：N + (N^0xFF)，双重发送确保传输可靠性
       * 实际读取字节数 = N + 1（N=0 读 1 字节，N=0xFF 读 256 字节） */
      OPENBL_USART_SendByte(ACK_BYTE);

      data = OPENBL_USART_ReadByte();
      xor  = ~data;

      if (OPENBL_USART_ReadByte() != xor)
      {
        /* 长度校验失败：补码不匹配 */
        OPENBL_USART_SendByte(NACK_BYTE);
      }
      else
      {
        /* ── 第4步：逐字节读取内存并发送 ── */
        OPENBL_USART_SendByte(ACK_BYTE);

        /* 根据地址查表获取存储器索引，确定使用哪个存储器驱动的 Read 回调 */
        memory_index = OPENBL_MEM_GetMemoryIndex(address);

        /* 循环读取 (N+1) 个字节，逐个发送给主机 */
        for (counter = ((uint32_t)data + 1U); counter != 0U; counter--)
        {
          OPENBL_USART_SendByte(OPENBL_MEM_Read(address, memory_index));
          address++;
        }
      }
    }
  }
}

/**
 * @brief  处理 Write Memory 命令（命令码 0x31），将主机发送的数据写入指定内存地址。
 *
 *         遵循 ST AN3155 USART 协议规范，交互流程为：
 *           1. MCU 收到命令码后，先检查读保护状态 ，回复 ACK 或 NACK
 *           2. 主机发送起始地址（4 字节 + XOR 校验），MCU 校验后回复 ACK
 *           3. 主机发送：长度 N + 数据[N+1 字节] + XOR 校验，MCU 校验后写入内存
 *           4. 若目标地址为选项字节区域，写入后自动触发 OBL 重载使配置生效
 *
 *         写入长度编码：N 的范围 0x00~0xFF，实际写入字节数 = N + 1（1~256 字节）
 *
 *         数据流：主机 → USART → USART_RAM_Buf 临时缓冲区 → OPENBL_MEM_Write → 目标存储器
 *         先将全部数据接收到 RAM 缓冲区并校验，校验通过后才执行实际写入，
 *         避免校验失败时已写入部分数据导致存储器内容不一致。
 *
 * @note   若读保护已使能（RDP Level != 0），直接回复 NACK 拒绝操作。
 *         写入选项字节后调用 OPENBL_MEM_OptionBytesLaunch() 触发 OBL 重载，
 *         这会导致系统复位以加载新的选项字节配置。
 *
 * @retval None
 */
static void OPENBL_USART_WriteMemory(void)
{
  uint32_t address;
  uint32_t tmpXOR;
  uint32_t counter;
  uint32_t codesize;
  uint32_t mem_area;
  uint8_t *ramaddress;
  uint8_t data;

  /* ── 第1步：读保护检查 ──
   * 若 RDP Level != 0（读保护已使能），拒绝写入并回复 NACK */
  if (OPENBL_MEM_GetReadOutProtectionStatus() != RESET)
  {
    OPENBL_USART_SendByte(NACK_BYTE);
  }
  else
  {
    /* ── 第2步：接收并校验目标地址 ──
     * 主机发送：addr[31:24] + addr[23:16] + addr[15:8] + addr[7:0] + XOR
     * GetAddress 内部校验 XOR 和地址合法性 */
    OPENBL_USART_SendByte(ACK_BYTE);

    if (OPENBL_USART_GetAddress(&address) == NACK_BYTE)
    {
      OPENBL_USART_SendByte(NACK_BYTE);
    }
    else
    {
      /* ── 第3步：接收数据到 RAM 临时缓冲区 ── */
      OPENBL_USART_SendByte(ACK_BYTE);

      /* 指向 USART 专用 RAM 缓冲区，用于暂存接收到的数据
       * 缓冲区大小 USART_RAM_BUFFER_SIZE = 1156 字节，足以容纳最大写入量 256 字节 */
      ramaddress = (uint8_t *)USART_RAM_Buf;

      /* 读取长度字节 N：实际写入字节数 = N + 1（范围 1~256） */
      data = OPENBL_USART_ReadByte();

      codesize = (uint32_t)data + 1U;

      /* XOR 校验和初始化：长度字节参与校验 */
      tmpXOR = data;

      /* 逐字节接收数据，存入 RAM 缓冲区，同时累加 XOR 校验和 */
      for (counter = codesize; counter != 0U ; counter--)
      {
        data    = OPENBL_USART_ReadByte();
        tmpXOR ^= data;

        *(__IO uint8_t *)(ramaddress) = data;

        ramaddress++;
      }

      /* ── 第4步：校验 XOR 完整性 ──
       * 主机在数据之后发送 1 字节校验和，与本地计算值比对 */
      if (OPENBL_USART_ReadByte() != tmpXOR)
      {
        /* 校验失败：丢弃缓冲区数据，回复 NACK */
        OPENBL_USART_SendByte(NACK_BYTE);
      }
      else
      {
        /* ── 第5步：校验通过，将缓冲区数据写入目标存储器 ──
         * OPENBL_MEM_Write 根据地址自动路由到对应存储器的 Write 回调
         * （Flash 双字编程 / RAM 直接写入 / OTP 编程 / OB 写入等） */
        OPENBL_MEM_Write(address, (uint8_t *)USART_RAM_Buf, codesize);

        OPENBL_USART_SendByte(ACK_BYTE);

        /* ── 第6步：选项字节特殊处理 ──
         * 若写入目标是选项字节区域（0x1FFF7800），需触发 OBL
         * （Option Byte Load）使新配置生效，这会导致系统复位 */
        mem_area = OPENBL_MEM_GetAddressArea(address);

        if (mem_area == OB_AREA)
        {
          OPENBL_MEM_OptionBytesLaunch();
        }
      }
    }
  }
}

/**
  * @brief  处理 Go 命令（命令码 0x21），跳转到指定地址执行用户应用程序。
  *
  *         遵循 ST AN3155 USART 协议规范，交互流程为：
  *           1. MCU 收到命令码后，先检查读保护状态 回复 ACK 或 NACK
  *           2. 主机发送跳转地址（4 字节 + XOR 校验），MCU 校验后回复 ACK
  *           3. MCU 检查地址区域合法性，仅允许跳转到 Flash 或 RAM
  *           4. 执行跳转，Bootloader 不再返回
  *
  *         跳转执行流程（OPENBL_MEM_JumpToAddress 内部）：
  *           - 反初始化 Bootloader 占用的硬件资源（USART、GPIO 等）
  *           - 使能全局中断
  *           - 从目标地址读取初始栈指针（MSP = *address）
  *           - 从 address+4 读取复位向量（PC = *(address+4)）
  *           - 跳转到用户应用执行
  *
  * @note   仅允许跳转到 FLASH_AREA 和 RAM_AREA，其他区域（OTP/OB/ICP/EB）
  *         回复 NACK 拒绝。这是因为在这些区域执行代码没有意义。
  *         跳转后 Bootloader 完全交出控制权，不再返回。
  *
  * @retval None
  */
static void OPENBL_USART_Go(void)
{
  uint32_t address;
  uint32_t mem_area;

  /* ── 第1步：读保护检查 ──
   * 若 RDP Level != 0（读保护已使能），拒绝跳转并回复 NACK */
  if (OPENBL_MEM_GetReadOutProtectionStatus() != RESET)
  {
    OPENBL_USART_SendByte(NACK_BYTE);
  }
  else
  {
    /* ── 第2步：接收并校验跳转地址 ──
     * 主机发送：addr[31:24] + addr[23:16] + addr[15:8] + addr[7:0] + XOR */
    OPENBL_USART_SendByte(ACK_BYTE);

    if (OPENBL_USART_GetAddress(&address) == NACK_BYTE)
    {
      OPENBL_USART_SendByte(NACK_BYTE);
    }
    else
    {
      /* ── 第3步：地址区域合法性检查 ──
       * 仅允许跳转到 Flash（用户固件）或 RAM（调试/RAM 应用），
       * OTP/OB/ICP/EB 等区域不可执行代码，回复 NACK */
      mem_area = OPENBL_MEM_GetAddressArea(address);

      if ((mem_area != FLASH_AREA) && (mem_area != RAM_AREA))
      {
        OPENBL_USART_SendByte(NACK_BYTE);
      }
      else
      {
        /* ── 第4步：校验通过，执行跳转 ──
         * 发送 ACK 后调用 JumpToAddress，此函数不会返回：
         *   1. OpenBootloader_DeInit() — 释放硬件资源
         *   2. Common_EnableIrq()      — 使能全局中断
         *   3. Common_SetMsp(*address) — 设置用户栈指针
         *   4. jump_to_address()       — 跳转到 *(address+4) 处执行 */
        OPENBL_USART_SendByte(ACK_BYTE);

        OPENBL_MEM_JumpToAddress(address);
      }
    }
  }
}

/**
  * @brief  处理 Readout Protect 命令（命令码 0x82），使能 Flash 读保护（RDP Level 1）。
  *
  *         遵循 ST AN3155 USART 协议规范，交互流程为：
  *           1. MCU 检查当前读保护状态，若已使能则回复 NACK
  *           2. 设置 RDP 为 Level 1，回复 ACK
  *           3. 触发 OBL 重载使配置生效，系统将自动复位
  *
  *         RDP 等级说明：
  *           - Level 0（0xAA）：无读保护，所有存储器可自由访问
  *           - Level 1（非 0xAA/0xCC）：读保护使能，Bootloader 仍可运行，
  *             但调试器无法读取 Flash 内容，从 RAM 启动时无法访问 Flash
  *           - Level 2（0xCC）：永久读保护，不可逆，调试器完全禁止连接
  *
  * @note   本命令将 RDP 从 Level 0 设置为 Level 1。
  *         OptionBytesLaunch() 触发 OBL 重载后系统会自动复位，
  *         复位后读保护生效，Bootloader 重新启动时 RDP != Level 0，
  *         ReadMemory/WriteMemory/Go/Erase 等命令将被拒绝（NACK）。
  *         若 RDP 已经是 Level 1 或 Level 2，本命令直接回复 NACK。
  *
  * @retval None
  */
static void OPENBL_USART_ReadoutProtect(void)
{
  /* ── 第1步：检查当前读保护状态 ──
   * 若 RDP 已非 Level 0（已处于 Level 1 或 Level 2），拒绝重复设置 */
  if (OPENBL_MEM_GetReadOutProtectionStatus() != RESET)
  {
    OPENBL_USART_SendByte(NACK_BYTE);
  }
  else
  {
    /* ── 第2步：使能读保护 ──
     * 将 RDP 选项字节从 0xAA（Level 0）改为其他值（Level 1），
     * 写入 FLASH->OPTR 寄存器的 RDP 字段 */
    OPENBL_USART_SendByte(ACK_BYTE);

    OPENBL_MEM_SetReadOutProtection(ENABLE);

    /* ── 第3步：回复 ACK 并触发 OBL 重载 ──
     * OptionBytesLaunch() 执行 OBL 重载（设置 FLASH->CR 的 OBL_LAUNCH 位），
     * 这会导致系统复位，新 RDP 配置在复位后生效。
     * 此函数不会返回。 */
    OPENBL_USART_SendByte(ACK_BYTE);

    OPENBL_MEM_OptionBytesLaunch();
  }
}

/**
  * @brief  处理 Readout Unprotect 命令（命令码 0x92），禁用 Flash 读保护（RDP Level 1 → Level 0）。
  *
  *         遵循 ST AN3155 USART 协议规范，交互流程为：
  *           1. MCU 回复 ACK（命令识别）
  *           2. MCU 立即回复第二个 ACK（在修改选项字节之前发送，因为后续操作会擦除 RAM）
  *           3. 将 RDP 从 Level 1 改回 Level 0（0xAA）
  *           4. 触发 OBL 重载，系统自动复位
  *
  *         ⚠️ 关键设计细节 — 两个 ACK 在 SetReadOutProtection 之前发送：
  *           RDP 从 Level 1 降级到 Level 0 时，Flash 内容会被自动擦除（安全措施），
  *           防止通过"先加保护再解除"的方式窃取固件。
  *           因此第二个 ACK 必须在调用 SetReadOutProtection 之前发送，
  *           确保 OBL 重载前主机已收到完整响应。
  *
  * @note   与 ReadoutProtect 不同，本函数不检查当前 RDP 状态，直接执行降级操作。
  *         RDP Level 2 是不可逆的，SetReadOutProtection(DISABLE) 在 Level 2 下无效，
  *         OBL 重载后系统复位，RDP 仍为 Level 2。
  *         RDP 从 Level 1 降级到 Level 0 时，Flash 内容会被自动擦除（安全措施），
  *         防止通过"先加保护再解除"的方式窃取固件。
  *
  * @retval None
  */
static void OPENBL_USART_ReadoutUnprotect(void)
{
  /* ── 第1个 ACK：命令识别确认 ── */
  OPENBL_USART_SendByte(ACK_BYTE);

  /* ── 第2个 ACK：操作完成确认（必须在 SetReadOutProtection 之前发送）──
   * RDP 降级后 OBL 重载会导致系统复位，此函数不会返回。
   * 因此第二个 ACK 必须提前发送，确保主机在系统复位前收到完整响应。 */
  OPENBL_USART_SendByte(ACK_BYTE);

  /* ── 将 RDP 从 Level 1 降级为 Level 0 ──
   * 写入 RDP = 0xAA（Level 0），解除读保护 */
  OPENBL_MEM_SetReadOutProtection(DISABLE);

  /* ── 触发 OBL 重载，系统自动复位 ──
   * 复位后 RDP = Level 0 生效，Flash 内容已被擦除（RDP 降级安全措施），
   * 此函数不会返回 */
  OPENBL_MEM_OptionBytesLaunch();
}

/**
  * @brief  处理 Extended Erase Memory 命令（命令码 0x44），擦除 Flash 指定页或全片擦除。
  *
  *         遵循 ST AN3155 USART 协议规范，支持两种擦除模式：
  *
  *         ┌─────────────────────────────────────────────────────────────┐
  *         │ 模式1：特殊擦除（页数编码高4位 = 0xF）                       │
  *         │   0xFFFF → 全片擦除（Mass Erase）                           │
  *         │   0xFFFE → Bank1 擦除                                      │
  *         │   0xFFFD → Bank2 擦除                                      │
  *         │   其他 0xFFFx → 不支持，回复 NACK                            │
  *         ├─────────────────────────────────────────────────────────────┤
  *         │ 模式2：页擦除（页数编码 < 0xFFF0）                           │
  *         │   页数 = N + 1（范围 1~65536 页）                            │
  *         │   随后发送 N+1 个页号（每个 2 字节，大端序）                    │
  *         └─────────────────────────────────────────────────────────────┘
  *
  *         交互流程：
  *           1. MCU 检查读保护状态，若已使能则 NACK
  *           2. 主机发送页数编码（2 字节，大端序）
  *           3. 根据编码进入特殊擦除或页擦除分支
  *           4. 主机发送 XOR 校验字节
  *           5. 校验通过后执行擦除，回复 ACK/NACK
  *
  *         USART_RAM_Buf 缓冲区格式（传递给 OPENBL_MEM_Erase/MassErase）：
  *           [0~1]  : 页数（小端序，低字节在前）
  *           [2~]   : 页号列表（每个页号 2 字节，小端序，低字节在前）
  *
  * @note   页号缓冲区大小受 USART_RAM_BUFFER_SIZE（1156 字节）限制，
  *         最多可存储 (1156-2)/2 = 577 个页号。若主机请求的页数超过此限制，
  *         超出部分的页号仅从 USART 读取（维持协议同步）但不存入缓冲区，
  *         实际擦除时只会处理缓冲区内的页号。
  *         STM32F072CBT6 共 64 页（128KB / 2KB 每页），577 的上限足够覆盖。
  *
  * @retval None
  */
static void OPENBL_USART_EraseMemory(void)
{
  uint32_t xor;
  uint32_t counter;
  uint32_t numpage;
  uint16_t data;
  ErrorStatus error_value;
  uint8_t status = ACK_BYTE;
  uint8_t *ramaddress;

  ramaddress = (uint8_t *) USART_RAM_Buf;

  /* ── 第1步：读保护检查 ──
   * 若 RDP Level != 0，拒绝擦除并回复 NACK */
  if (OPENBL_MEM_GetReadOutProtectionStatus() != RESET)
  {
    OPENBL_USART_SendByte(NACK_BYTE);
  }
  else
  {
    OPENBL_USART_SendByte(ACK_BYTE);

    /* ── 第2步：接收页数编码（2 字节，大端序）──
     * 第1字节为高8位，第2字节为低8位 */
    data = OPENBL_USART_ReadByte();
    data = (uint16_t)(data << 8) | OPENBL_USART_ReadByte();

    /* XOR 校验和初始化：页数编码的两个字节参与校验 */
    xor  = ((uint32_t)data & 0xFF00U) >> 8;
    xor ^= (uint32_t)data & 0x00FFU;

    /* ── 第3步：根据页数编码判断擦除模式 ── */

    /* ═══════ 模式1：特殊擦除（页数编码高4位 = 0xF）═══════ */
    if ((data & 0xFFF0U) == 0xFFF0U)
    {
      /* 校验 XOR 完整性 */
      if (OPENBL_USART_ReadByte() != (uint8_t) xor)
      {
        status = NACK_BYTE;
      }
      else
      {
        /* 0xFFFF=全片擦除, 0xFFFE=Bank1擦除, 0xFFFD=Bank2擦除 */
        if ((data == 0xFFFFU) || (data == 0xFFFEU) || (data == 0xFFFDU))
        {
          /* 将特殊擦除编码存入缓冲区（小端序：低字节在前） */
          ramaddress[0] = (uint8_t)(data & 0x00FFU);
          ramaddress[1] = (uint8_t)((data & 0xFF00U) >> 8);

          /* 执行全片/Bank 擦除，通过缓冲区传递擦除类型编码 */
          error_value = OPENBL_MEM_MassErase(FLASH_START_ADDRESS, (uint8_t *) USART_RAM_Buf, USART_RAM_BUFFER_SIZE);

          if (error_value == SUCCESS)
          {
            status = ACK_BYTE;
          }
          else
          {
            status = NACK_BYTE;
          }
        }
        else
        {
          /* 其他 0xFFFx 编码不支持 */
          status = NACK_BYTE;
        }
      }
    }
    /* ═══════ 模式2：页擦除（页数编码 < 0xFFF0）═══════ */
    else
    {
      ramaddress = (uint8_t *) USART_RAM_Buf;

      /* 实际页数 = 编码值 + 1（范围 1~65536） */
      numpage = (uint32_t)data + 1U;

      /* 缓冲区前2字节存放页数（小端序） */
      *ramaddress = (uint8_t)(numpage & 0x00FFU);
      ramaddress++;

      *ramaddress = (uint8_t)((numpage & 0xFF00U) >> 8);
      ramaddress++;

      /* 逐个接收页号，每个页号占 2 字节（大端序），存入缓冲区时转为小端序 */
      for (counter = numpage; counter != 0U ; counter--)
      {
        /* 接收页号高字节（MSB），同时参与 XOR 校验 */
        data  = OPENBL_USART_ReadByte();
        xor  ^= data;
        data  = (uint16_t)((data & 0x00FFU) << 8);

        /* 接收页号低字节（LSB），拼成完整页号，同时参与 XOR 校验 */
        data |= (uint8_t)(OPENBL_USART_ReadByte() & 0x00FFU);
        xor  ^= ((uint32_t)data & 0x00FFU);

        /* 缓冲区容量保护：仅存储能放入缓冲区的页号
         * USART_RAM_BUFFER_SIZE=1156, 减去2字节页数头, 每页号2字节,
         * 最多存 (1156-2)/2 = 577 个页号。超出部分仅读取维持协议同步 */
        if (counter < (USART_RAM_BUFFER_SIZE / 2U))
        {
          *ramaddress = (uint8_t)(data & 0x00FFU);
          ramaddress++;

          *ramaddress = (uint8_t)((data & 0xFF00U) >> 8);
          ramaddress++;
        }
        else
        {
          /* 页号超出缓冲区容量，仅从 USART 读取丢弃，维持协议帧同步 */
        }

      }

      /* 校验 XOR 完整性 */
      if (OPENBL_USART_ReadByte() != (uint8_t) xor)
      {
        status = NACK_BYTE;
      }
      else
      {
        /* 校验通过，执行页擦除
         * OPENBL_MEM_Erase 解析缓冲区格式：[页数(2B)] [页号1(2B)] [页号2(2B)] ... */
        error_value = OPENBL_MEM_Erase(FLASH_START_ADDRESS, (uint8_t *) USART_RAM_Buf, USART_RAM_BUFFER_SIZE);

        if (error_value == SUCCESS)
        {
          status = ACK_BYTE;
        }
      }
    }

    /* ── 第4步：发送最终状态（ACK 或 NACK）── */
    OPENBL_USART_SendByte(status);
  }
}

/**
  * @brief  处理 Write Protect 命令（命令码 0x63），使能 Flash 指定页的写保护（WRP）。
  *
  *         遵循 ST AN3155 USART 协议规范，交互流程为：
  *           1. MCU 检查读保护状态，若已使能则 NACK
  *           2. 主机发送：长度 N + 页号列表[N+1 字节] + XOR 校验
  *           3. MCU 校验通过后设置 WRP 写保护，回复 ACK
  *           4. 触发 OBL 重载使配置生效（系统复位）
  *
  *         写保护（WRP）机制说明：
  *           - WRP 通过选项字节寄存器（FLASH->WRP1AR/WRP1BR/WRP2AR/WRP2BR）
  *             控制指定 Flash 页是否允许擦除/编程操作
  *           - 被保护的页无法通过任何方式擦写，直到 WRP 被禁用
  *           - 常用于保护 Bootloader 区域或关键数据区不被意外覆盖
  *
  *         USART_RAM_Buf 缓冲区内容：
  *           [0]     : 第1个要保护的页号
  *           [1]     : 第2个要保护的页号
  *           ...
  *           [N]     : 第N+1个要保护的页号
  *           每个页号占 1 字节，页数 = N + 1（范围 1~256 页）
  *
  * @note   长度编码与 ReadMemory/WriteMemory 相同：N 范围 0x00~0xFF，
  *         实际页数 = N + 1（1~256 页）。
  *         写保护设置成功后调用 OptionBytesLaunch() 触发 OBL 重载，
  *         系统将自动复位，新的 WRP 配置在复位后生效。
  *         注意：第二个 ACK 在 OBL 重载之前发送，确保主机收到响应。
  *
  * @retval None
  */
static void OPENBL_USART_WriteProtect(void)
{
  uint32_t counter;
  uint32_t length;
  uint32_t data;
  uint32_t xor;
  ErrorStatus error_value;
  uint8_t *ramaddress;

  /* ── 第1步：读保护检查 ──
   * 若 RDP Level != 0（读保护已使能），拒绝操作并回复 NACK */
  if (OPENBL_MEM_GetReadOutProtectionStatus() != RESET)
  {
    OPENBL_USART_SendByte(NACK_BYTE);
  }
  else
  {
    OPENBL_USART_SendByte(ACK_BYTE);

    /* ── 第2步：接收页号数据到 RAM 缓冲区 ── */

    /* 读取长度字节 N：实际页数 = N + 1（范围 1~256） */
    data = OPENBL_USART_ReadByte();

    ramaddress = (uint8_t *) USART_RAM_Buf;
    length     = data + 1U;

    /* XOR 校验和初始化：长度字节参与校验 */
    xor = data;

    /* 逐字节接收页号列表，存入 RAM 缓冲区，同时累加 XOR 校验和
     * 每个字节代表一个要保护的 Flash 页号（0~127 对应 128 页） */
    for (counter = length; counter != 0U ; counter--)
    {
      data  = OPENBL_USART_ReadByte();
      xor  ^= data;

      *(__IO uint8_t *)(ramaddress) = (uint8_t) data;

      ramaddress++;
    }

    /* ── 第3步：校验 XOR 完整性 ──
     * 主机在页号列表之后发送 1 字节校验和 */
    if (OPENBL_USART_ReadByte() != (uint8_t) xor)
    {
      OPENBL_USART_SendByte(NACK_BYTE);
    }
    else
    {
      ramaddress = (uint8_t *) USART_RAM_Buf;

      /* ── 第4步：设置写保护 ──
       * OPENBL_MEM_SetWriteProtection 将缓冲区中的页号列表转换为
       * WRP 选项字节寄存器值（FLASH->WRP1AR/WRP1BR/WRP2AR/WRP2BR），
       * 并写入选项字节区域 */
      error_value = OPENBL_MEM_SetWriteProtection(ENABLE, FLASH_START_ADDRESS, ramaddress, length);

      /* 在 OBL 重载之前发送 ACK，确保主机收到响应
       * （OBL 重载会导致系统复位，之后无法再发送数据） */
      OPENBL_USART_SendByte(ACK_BYTE);

      /* ── 第5步：触发 OBL 重载 ──
       * 仅当 SetWriteProtection 成功时才触发 OBL 重载，
       * 使新的 WRP 配置生效。系统将自动复位，此函数不会返回。 */
      if (error_value == SUCCESS)
      {
        OPENBL_MEM_OptionBytesLaunch();
      }
    }
  }
}

/**
  * @brief  处理 Write Unprotect 命令（命令码 0x73），禁用 Flash 所有页的写保护（WRP）。
  *
  *         遵循 ST AN3155 USART 协议规范，交互流程为：
  *           1. MCU 检查读保护状态，若已使能则 NACK
  *           2. MCU 清除所有 WRP 写保护位，回复 ACK
  *           3. 触发 OBL 重载使配置生效（系统复位）
  *
  *         与 WriteProtect 命令的区别：
  *           - WriteProtect：指定页号列表，使能对应页的写保护
  *           - WriteUnprotect：无需页号列表，一次性清除所有页的写保护
  *             （传入 NULL 和 0 表示全部解除）
  *
  *         底层实现：
  *           SetWriteProtection(DISABLE) 将所有 WRP 选项字节寄存器
  *           （FLASH->WRP1AR/WRP1BR/WRP2AR/WRP2BR）写为默认值（0xFF），
  *           即所有页均不受写保护。
  *
  * @note   第二个 ACK 在 OBL 重载之前发送，与 ReadoutUnprotect 同理：
  *         OBL 重载会导致系统复位，必须在复位前确保主机收到完整响应。
  *         若 SetWriteProtection 返回错误（如 Flash 操作失败），
  *         则不触发 OBL 重载，系统不会复位，Bootloader 继续运行。
  *
  * @retval None
  */
static void OPENBL_USART_WriteUnprotect(void)
{
  ErrorStatus error_value;

  /* ── 第1步：读保护检查 ──
   * 若 RDP Level != 0（读保护已使能），拒绝操作并回复 NACK */
  if (OPENBL_MEM_GetReadOutProtectionStatus() != RESET)
  {
    OPENBL_USART_SendByte(NACK_BYTE);
  }
  else
  {
    /* ── 第2步：命令识别确认 ── */
    OPENBL_USART_SendByte(ACK_BYTE);

    /* ── 第3步：清除所有 WRP 写保护 ──
     * 传入 DISABLE + NULL + 0，表示解除全部页的写保护
     * 底层将 WRP1AR/WRP1BR/WRP2AR/WRP2BR 全部写为 0xFF（无保护） */
    error_value = OPENBL_MEM_SetWriteProtection(DISABLE, FLASH_START_ADDRESS, NULL, 0);

    /* ── 第4步：在 OBL 重载之前发送 ACK ──
     * OBL 重载会导致系统复位，必须先发送 ACK 确保主机收到响应 */
    OPENBL_USART_SendByte(ACK_BYTE);

    /* ── 第5步：触发 OBL 重载 ──
     * 仅当 SetWriteProtection 成功时才触发，使新的 WRP 配置生效
     * 系统将自动复位，此函数不会返回 */
    if (error_value == SUCCESS)
    {
      OPENBL_MEM_OptionBytesLaunch();
    }
  }
}

/**
  * @brief  从主机接收 4 字节目标地址并校验其合法性与完整性。
  *
  *         遵循 ST AN3155 USART 协议规范，主机发送的地址帧格式为：
  *           addr[31:24] + addr[23:16] + addr[15:8] + addr[7:0] + XOR
  *         共 5 字节，高字节在前（大端序），最后 1 字节为前 4 字节的异或校验和。
  *
  *         校验分两层：
  *           1. XOR 完整性校验：确保传输过程中数据未出错
  *           2. 地址合法性校验：通过存储器描述符表检查地址是否落在已注册的
  *              存储器范围内（Flash/RAM/OTP/OB 等），AREA_ERROR 表示无效地址
  *
  * @param  Address 输出参数，校验通过时写入组合后的 32 位目标地址
  * @retval ACK_BYTE (0x79) 地址校验通过
  *         NACK_BYTE (0x1F) XOR 校验失败或地址不在合法存储器范围内
  */
static uint8_t OPENBL_USART_GetAddress(uint32_t *Address)
{
  uint8_t data[4] = {0, 0, 0, 0};
  uint8_t status;
  uint8_t xor;

  /* ── 接收 4 字节地址，大端序：先收高字节 ──
   * data[3] = addr[31:24]（最高字节）
   * data[2] = addr[23:16]
   * data[1] = addr[15:8]
   * data[0] = addr[7:0]（最低字节） */
  data[3] = OPENBL_USART_ReadByte();
  data[2] = OPENBL_USART_ReadByte();
  data[1] = OPENBL_USART_ReadByte();
  data[0] = OPENBL_USART_ReadByte();

  /* 计算前 4 字节的异或校验和 */
  xor = data[3] ^ data[2] ^ data[1] ^ data[0];

  /* ── 第1层校验：XOR 完整性 ──
   * 第 5 字节为主机发送的校验和，与本地计算值比对 */
  if (OPENBL_USART_ReadByte() != xor)
  {
    status = NACK_BYTE;
  }
  else
  {
    /* ── 组合 32 位地址（大端序 → 小端序重组） ── */
    *Address = ((uint32_t)data[3] << 24) | ((uint32_t)data[2] << 16) | ((uint32_t)data[1] << 8) | (uint32_t)data[0];

    /* ── 第2层校验：地址合法性 ──
     * 遍历已注册的存储器描述符表，检查地址是否落在某个存储器范围内。
     * 若不在任何已注册存储器范围内，返回 AREA_ERROR */
    if (OPENBL_MEM_GetAddressArea(*Address) == AREA_ERROR)
    {
      status = NACK_BYTE;
    }
    else
    {
      status = ACK_BYTE;
    }
  }

  return status;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
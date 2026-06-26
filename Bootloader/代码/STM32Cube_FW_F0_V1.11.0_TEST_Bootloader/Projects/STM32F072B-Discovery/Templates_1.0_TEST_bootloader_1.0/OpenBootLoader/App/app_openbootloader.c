/**
  ******************************************************************************
  * @file    app_openbootloader.c
  * @author  MCD Application Team
  * @brief   OpenBootloader application entry point
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
#include "main.h"
#include "app_openbootloader.h"
#include "usart_interface.h"
#include "flash_interface.h"
#include "ram_interface.h"
#include "optionbytes_interface.h"
#include "iwdg_interface.h"
#include "openbl_usart_cmd.h"
#include "openbl_core.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static OPENBL_HandleTypeDef USART_Handle;
static OPENBL_HandleTypeDef IWDG_Handle;

static OPENBL_OpsTypeDef USART_Ops =
{
  OPENBL_USART_Configuration, // 初始化 USART 接口
  NULL,  // 反初始化函数
  OPENBL_USART_ProtocolDetection, // 协议检测函数
  OPENBL_USART_GetCommandOpcode, // 获取命令码函数
  OPENBL_USART_SendByte // 发送字节函数
};

static OPENBL_OpsTypeDef IWDG_Ops =
{
  OPENBL_IWDG_Configuration,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  初始化 Open Bootloader。
  *         本函数执行两阶段初始化：
  *         阶段1 - 将所有通信接口（USART、IWDG）注册到核心调度表中，
  *                 每个接口绑定各自的操作回调函数集和命令处理列表。
  *         阶段2 - 调用已注册接口的初始化回调完成硬件配置，
  *                 然后将所有存储器描述符（Flash、RAM、选项字节、OTP、系统存储器、
  *                 工程字节）注册到内存管理模块。
  * @param  无
  * @retval 无
  */
void OpenBootloader_Init(void)
{
  /* ---- 阶段1：将通信接口注册到核心调度表 ---- */

  /* 注册 USART 接口：绑定操作回调（初始化/协议检测/获取命令码/发送字节）
     及 USART 专用命令处理列表（Get/Version/ID/Read/Write/Go/Erase/Protect 等） */
  USART_Handle.p_Ops = &USART_Ops;
  USART_Handle.p_Cmd = OPENBL_USART_GetCommandsList();
  OPENBL_RegisterInterface(&USART_Handle);

  /* 注册 IWDG 接口：仅绑定初始化回调用于看门狗配置，
     无协议检测和命令处理（IWDG 不是通信接口，仅用于喂狗） */
  IWDG_Handle.p_Ops = &IWDG_Ops;
  IWDG_Handle.p_Cmd = NULL;
  OPENBL_RegisterInterface(&IWDG_Handle);

  /* ---- 阶段2：初始化硬件并注册存储器描述符 ---- */

  /* 调用所有已注册接口的 Init() 回调完成硬件初始化
     （如配置 USART 引脚/波特率等） */
  OPENBL_Init();

   /* 将存储器描述符注册到内存管理模块。
     每个描述符包含：地址范围、类型标识、以及读/写/擦除/保护/跳转等回调函数。
     命令处理层通过统一接口访问任意存储区域，无需关心底层细节。 */
  OPENBL_MEM_RegisterMemory(&FLASH_Descriptor);    /* 主 Flash：0x08000000，128KB */
  OPENBL_MEM_RegisterMemory(&RAM_Descriptor);      /* SRAM：0x20000000，16KB */
  OPENBL_MEM_RegisterMemory(&OB_Descriptor);       /* 选项字节：0x1FFFF800，16B */
//   OPENBL_MEM_RegisterMemory(&OTP_Descriptor);      /* 一次性可编程区：0x1FFF7000，1KB */
//   OPENBL_MEM_RegisterMemory(&ICP1_Descriptor);     /* 系统存储器 Bank1：0x1FFF0000 包含 ST 原厂 Bootloader 固件*/
//   OPENBL_MEM_RegisterMemory(&ICP2_Descriptor);     /* 系统存储器 Bank2：0x1FFF8000 */
//   OPENBL_MEM_RegisterMemory(&EB_Descriptor);       /* 工程字节：0x1FFF7500，64B   ST 在生产测试阶段使用的只读寄存器区域*/
}

/**
  * @brief  反初始化 Open Bootloader。
  *         在 GO 命令跳转到用户应用程序前调用（由 OPENBL_FLASH_JumpToAddress 间接调用），
  *         将系统时钟等硬件配置恢复为复位默认值，确保用户应用启动时
  *         不受 Bootloader 运行期间配置残留的影响。
  * @param  无
  * @retval 无
  */
void OpenBootloader_DeInit(void)
{
  /* 调用系统级反初始化，将 RCC 时钟配置恢复为复位状态 */
  HAL_DeInit();
}

/**
  * @brief  协议检测与命令处理循环。
  *         在 main() 的 while(1) 中被反复调用，执行两步逻辑：
  *         1. 未检测到接口时：轮询所有已注册接口的 Detection() 回调，
  *            等待主机发起通信（如 USART 收到 0x7F 同步字节），
  *            一旦检测到活动即锁定该接口，后续不再重复检测。
  *         2. 已检测到接口后：持续从锁定的接口读取命令操作码并分发执行，
  *            直到 GO 命令跳转到用户应用程序或系统复位。
  * @param  无
  * @retval 无
  */
void OpenBootloader_ProtocolDetection(void)
{
  /* 静态变量，记录是否已检测到活跃的通信接口
     0 = 尚未检测到，1 = 已检测到并锁定接口 */
  static uint32_t interface_detected = 0;

  /* 阶段1：接口检测（仅执行一次）
     轮询所有已注册接口的 Detection() 回调，一旦某接口检测到主机通信活动，
     核心层将锁定该接口并返回 1，后续不再进入此分支 */
  if (interface_detected == 0)
  {
    interface_detected = OPENBL_InterfaceDetection();
  }

  /* 阶段2：命令处理（检测到接口后持续执行）
     从锁定的接口获取命令操作码，通过 switch 分发到对应的命令处理函数
     （如 Read/Write/Erase/Go 等），循环处理直到跳转或复位 */
  if (interface_detected == 1)
  {
    OPENBL_CommandProcess();
  }
}
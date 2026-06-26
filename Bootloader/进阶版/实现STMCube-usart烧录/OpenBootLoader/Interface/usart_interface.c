/**
  ******************************************************************************
  * @file    usart_interface.c
  * @author  MCD Application Team
  * @brief   Contains USART HW configuration
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
#include "openbl_core.h"
#include "openbl_usart_cmd.h"
#include "usart_interface.h"
#include "iwdg_interface.h"
#include "interfaces_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void OPENBL_USART_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  初始化 USART 外设，配置串口通信参数
  *
  *         通信参数：115200 波特率，9 位数据宽度，1 停止位，偶校验，无硬件流控
  *         实际有效数据为 8 位 + 1 位校验位，故数据宽度设为 9B。
  *
  *         自动波特率检测机制：
  *         STM32CubeProgrammer 通过 USART 连接时，首先发送 0x7F 同步字节，
  *         USART 硬件通过测量该字节的位宽自动计算实际波特率，无需预先约定。
  *         若芯片不支持自动波特率检测，则回退到固定 115200 波特率。
  *
  * @retval 无
  */
static void OPENBL_USART_Init(void)
{
  LL_USART_InitTypeDef USART_InitStruct;

  /* 串口参数配置 */
  USART_InitStruct.BaudRate            = 115200U;                   /* 波特率 115200（自动波特率模式下此值为标称值） */
  USART_InitStruct.DataWidth           = LL_USART_DATAWIDTH_9B;     /* 9位数据宽度：8位数据 + 1位偶校验位 */
  USART_InitStruct.StopBits            = LL_USART_STOPBITS_1;       /* 1位停止位 */
  USART_InitStruct.Parity              = LL_USART_PARITY_EVEN;      /* 偶校验 */
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;   /* 无硬件流控 */
  USART_InitStruct.TransferDirection   = LL_USART_DIRECTION_TX_RX;  /* 全双工收发 */
  USART_InitStruct.OverSampling        = LL_USART_OVERSAMPLING_16;  /* 16倍过采样，提高采样精度 */

  /* 自动波特率检测：主机发送 0x7F 同步字节，硬件自动测量位宽计算波特率 */
  if (IS_USART_AUTOBAUDRATE_DETECTION_INSTANCE(USARTx))
  {
    LL_USART_EnableAutoBaudRate(USARTx);                                    /* 使能自动波特率检测 */
    LL_USART_SetAutoBaudRateMode(USARTx, LL_USART_AUTOBAUD_DETECT_ON_7F_FRAME); /* 以 0x7F 帧为检测依据 */
  }
  else
  {
    LL_USART_DisableAutoBaudRate(USARTx);   /* 不支持自动波特率，禁用 */
    USART_InitStruct.BaudRate = 115200;      /* 回退到固定 115200 波特率 */
  }

  /* 应用配置并使能 USART */
  LL_USART_Init(USARTx, &USART_InitStruct);
  LL_USART_Enable(USARTx);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  配置 USART 引脚和外设，完成串口初始化
  *
  *         初始化流程：
  *         1. 使能 GPIO 和 USART 时钟
  *         2. 配置 TX/RX 引脚为复用推挽输出、上拉、高速模式
  *         3. 调用 OPENBL_USART_Init() 初始化 USART 通信参数
  *
  * @retval 无
  */
void OPENBL_USART_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* 使能 GPIOA 时钟（USART1 的 TX/RX 引脚位于 GPIOA） */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* 使能 USART1 时钟 */
  __HAL_RCC_USART1_CLK_ENABLE();

  /* 配置 USART TX 引脚：复用推挽、上拉、高速、映射到 USART 复用功能 */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;       /* 复用推挽输出 */
  GPIO_InitStruct.Pull      = GPIO_PULLUP;            /* 上拉，空闲态为高电平 */
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;   /* 高速，满足高波特率要求 */
  GPIO_InitStruct.Alternate = USARTx_ALTERNATE;       /* 映射到 USART 复用功能 */
  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* 配置 USART RX 引脚：复用 TX 的其余参数，仅替换引脚号和端口 */
  GPIO_InitStruct.Pin = USARTx_RX_PIN;
  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

  /* 初始化 USART 通信参数（波特率、校验位、自动波特率检测等） */
  OPENBL_USART_Init();
}

/**
  * @brief  USART 协议检测，判断主机是否通过 USART 发起通信。
  *         检测 USART 自动波特率标志位，判断是否收到有效的 0x7F 同步字节：
  *         - ABRF=1 且 ABRE=0：自动波特率检测成功，主机已发起通信
  *         - ABRF=1 且 ABRE=1：自动波特率检测失败，同步帧无效
  *         - ABRF=0：尚未收到同步字节
  *         检测成功后，读取并丢弃同步字节，向主机回复 ACK 完成握手。
  * @retval detected: 0=未检测到通信活动，1=已检测到主机通信
  */
uint8_t OPENBL_USART_ProtocolDetection(void)
{
  uint8_t detected;

  /* 检查 USART 自动波特率检测结果：
     ABRF=1（自动波特率检测完成）且 ABRE=0（无错误），说明收到了有效的 0x7F 同步帧 */
  if (((USARTx->ISR & LL_USART_ISR_ABRF) != 0) && ((USARTx->ISR & LL_USART_ISR_ABRE) == 0))
  {
    /* 读取并丢弃接收到的 0x7F 同步字节，清空接收缓冲区 */
    OPENBL_USART_ReadByte();

    /* 向主机回复 ACK（0x79），确认握手成功 */
    OPENBL_USART_SendByte(ACK_BYTE);

    detected = 1;
  }
  else
  {
    detected = 0;
  }

  return detected;
}

/**
  * @brief  从主机获取命令操作码
  *
  *         STM32 Bootloader USART 协议中，主机发送命令时连续发送两个字节：
  *         - 第1字节：命令操作码（command opcode）
  *         - 第2字节：命令操作码的补码（command opcode complement）
  *
  *         校验规则：opcode XOR complement = 0xFF，即两个字节互为按位取反。
  *         若校验失败，返回 ERROR_COMMAND 表示通信错误。
  *
  * @retval 命令操作码，校验失败时返回 ERROR_COMMAND
  */
uint8_t OPENBL_USART_GetCommandOpcode(void)
{
  uint8_t command_opc = 0x0;

  /* 读取第1字节：命令操作码 */
  command_opc = OPENBL_USART_ReadByte();

  /* 读取第2字节并与第1字节异或，校验数据完整性
   * 正确时 opcode ^ complement = 0xFF（互为按位取反） */
  if ((command_opc ^ OPENBL_USART_ReadByte()) != 0xFF)
  {
    command_opc = ERROR_COMMAND;
  }

  return command_opc;
}

/**
  * @brief  从 USART 读取一个字节
  *
  *         轮询等待 RXNE 标志位（接收缓冲区非空），表示新数据已就绪。
  *         等待期间持续喂狗，防止 IWDG 超时复位。
  *
  * @retval 读取到的字节
  */
uint8_t OPENBL_USART_ReadByte(void)
{
  /* 等待接收缓冲区非空（RXNE=1），期间持续喂狗防止看门狗复位 */
  while (!LL_USART_IsActiveFlag_RXNE(USARTx))
  {
    OPENBL_IWDG_Refresh();
  }

  /* 读取接收数据寄存器，同时自动清除 RXNE 标志 */
  return LL_USART_ReceiveData8(USARTx);
}

/**
  * @brief  通过 USART 发送一个字节
  *
  *         先将数据写入发送数据寄存器，再等待 TC 标志位（发送完成），
  *         确保字节完整发送后再返回，避免后续操作覆盖发送缓冲区。
  *
  * @param  Byte 待发送的字节
  * @retval 无
  */
void OPENBL_USART_SendByte(uint8_t Byte)
{
  /* 写入发送数据寄存器，启动发送 */
  LL_USART_TransmitData8(USARTx, (Byte & 0xFF));

  /* 等待 TC=1（发送完成），确保字节完整移出后再返回 */
  while (!LL_USART_IsActiveFlag_TC(USARTx))
  {
  }
}
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    quadspi.c
 * @brief   This file provides code for the configuration
 *          of the QUADSPI instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "quadspi.h"

/* USER CODE BEGIN 0 */

volatile uint8_t qspi_tx_done = 0; // 标志位：数据写入完成
volatile uint8_t qspi_rx_done = 0; // 标志位：数据读取完成
/* USER CODE END 0 */

QSPI_HandleTypeDef hqspi;
DMA_HandleTypeDef hdma_quadspi;

// 初始化QUADSPI
/* QUADSPI init function */  
void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  hqspi.Instance = QUADSPI; 
  hqspi.Init.ClockPrescaler = 0; // 时钟不分频（最高速）
  hqspi.Init.FifoThreshold = 16; // FIFO 阈值 16 字节
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE; // 半周期采样偏移
  hqspi.Init.FlashSize = 24; // 外部 Flash 大小 2^24 = 16MB
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE; // CS 高电平保持 2 个时钟周期
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;// 时钟模式 0
  hqspi.Init.FlashID = QSPI_FLASH_ID_1; // 外部 Flash ID 1
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE; // 禁用双 Flash 模式 // 单 Flash 模式
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */
}

// 配置QUADSPI的GPIO  MX_QUADSPI_Init初始化后调用的硬件配置
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *qspiHandle)
{
	
  __HAL_RCC_DMA2_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (qspiHandle->Instance == QUADSPI)
  {
    /* USER CODE BEGIN QUADSPI_MspInit 0 */

    /* USER CODE END QUADSPI_MspInit 0 */
    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PA1     ------> QUADSPI_BK1_IO3
    PB1     ------> QUADSPI_CLK
    PC8     ------> QUADSPI_BK1_IO2
    PC9     ------> QUADSPI_BK1_IO0
    PC10     ------> QUADSPI_BK1_IO1
    PB6     ------> QUADSPI_BK1_NCS
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* QUADSPI DMA Init */
    /* QUADSPI Init */
    hdma_quadspi.Instance = DMA2_Stream7; //QSPI 固定映射到这个流
    hdma_quadspi.Init.Channel = DMA_CHANNEL_3; // DMA 通道 3
    hdma_quadspi.Init.Direction = DMA_MEMORY_TO_PERIPH; // DMA 方向 从内存到外设
    hdma_quadspi.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不自增
    hdma_quadspi.Init.MemInc = DMA_MINC_ENABLE; // 内存地址自增
    hdma_quadspi.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // 外设数据对齐 8位 按字节（8位）
    hdma_quadspi.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; // 内存数据对齐 8位 按字节（8位）
    hdma_quadspi.Init.Mode = DMA_NORMAL; // DMA 模式 单次模式
    hdma_quadspi.Init.Priority = DMA_PRIORITY_HIGH; // DMA 优先级 高
    hdma_quadspi.Init.FIFOMode = DMA_FIFOMODE_ENABLE; // DMA FIFO 模式 使能
    hdma_quadspi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL; // DMA FIFO 阈值 半满 8
    hdma_quadspi.Init.MemBurst = DMA_MBURST_INC4; // 突发传输 4 拍（一次突发传 4×1=4 字节）
    hdma_quadspi.Init.PeriphBurst = DMA_PBURST_INC4; // 突发传输 4 拍（一次突发传 4×1=4 字节）
    if (HAL_DMA_Init(&hdma_quadspi) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(qspiHandle, hdma, hdma_quadspi);

    /* QUADSPI interrupt Init */
    HAL_NVIC_SetPriority(QUADSPI_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
    /* USER CODE BEGIN QUADSPI_MspInit 1 */

    /* USER CODE END QUADSPI_MspInit 1 */
		
		
  /* DMA2_Stream7_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
  }
}

// 释放QUADSPI的GPIO  MX_QUADSPI_Init初始化后调用的硬件配置
void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef *qspiHandle)
{

  if (qspiHandle->Instance == QUADSPI)
  {
    /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

    /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration
    PA1     ------> QUADSPI_BK1_IO3
    PB1     ------> QUADSPI_CLK
    PC8     ------> QUADSPI_BK1_IO2
    PC9     ------> QUADSPI_BK1_IO0
    PC10     ------> QUADSPI_BK1_IO1
    PB6     ------> QUADSPI_BK1_NCS
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1 | GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);

    /* QUADSPI DMA DeInit */
    HAL_DMA_DeInit(qspiHandle->hdma);

    /* QUADSPI interrupt Deinit */
    HAL_NVIC_DisableIRQ(QUADSPI_IRQn);
    /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

    /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}

// 初始化QUADSPI  无使用
void QSPI_Init()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_QSPI_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_QSPI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
//恢复GPIO配置
void QSPI_DeInit()
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);

  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_6, 0);

  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10, 0);
}
/* USER CODE BEGIN 1 */
// 用硬件抽象层包装hal库发送数�?
void qspi_instruct(uint8_t *date, uint16_t length)
{
  QSPI_CommandTypeDef sCommand;
  sCommand.InstructionMode = QSPI_INSTRUCTION_NONE;  // 无指令模式
  sCommand.Instruction = 0x00; // 写命�?      // 指令字节
  sCommand.AddressMode = QSPI_ADDRESS_NONE;    // 无地址模式
  sCommand.AddressSize = QSPI_ADDRESS_24_BITS;   // 地址大小 24 位
  sCommand.Address = 0;   // 地址值
  sCommand.DataMode = QSPI_DATA_1_LINE;    // 单线数据模式
  sCommand.DummyCycles = 0;     // 空闲周期 0
  sCommand.NbData = length;    // 数据长度
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  // 无备用字节
  sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;    // 禁用 DDR 模式
  sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;// DDR 保持半周期
  sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;   // 每次命令都发送指令

   // 2. 发送 QSPI 命令（配置传输参数）
  if (HAL_QSPI_Command(&hqspi, &sCommand, 1000) != HAL_OK)
  {
    Error_Handler();
  }
  /*
  HAL_QSPI_Command(&hqspi, &sCommand, 1000)  ← 配置传输参数
  ├── 使用 hqspi 句柄（MX_QUADSPI_Init 初始化）
  └── 配置 QSPI 寄存器
  */

  // 3. 通过 DMA 发送数据   使用 DMA2_Stream7（由 HAL_QSPI_MspInit 配置）
  if (HAL_QSPI_Transmit_DMA(&hqspi, date) != HAL_OK)
  {
    Error_Handler();
  }
  /*
  HAL_QSPI_Transmit_DMA(&hqspi, date)  ← 启动 DMA 传输
  ├── 使用 hqspi->hdma（HAL_QSPI_MspInit 配置的 DMA2_Stream7）
  ├── 配置 DMA 源地址：date（内存）
  ├── 配置 DMA 目标地址：QSPI 数据寄存器
  ├── 配置 DMA 传输长度：5 字节
  └── 启动 DMA 传输（非阻塞）

  DMA2_Stream7 自动传输数据
  ├── 从内存读取数据
  ├── 通过 QSPI GPIO 引脚发送
  └── 传输到 OLED 显示屏
  */
}

/* USER CODE BEGIN 1 */
// 用硬件抽象层包装hal库发送数�?
void qspi_send_message(uint8_t *date, uint16_t length)
{
  QSPI_CommandTypeDef sCommand;
  sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE; // 单线指令模式       ------
  sCommand.Instruction = 0x32; // 写命令（图像数据命令）        ------
  sCommand.AddressMode = QSPI_ADDRESS_NONE;  // 无地址模式
  sCommand.AddressSize = QSPI_ADDRESS_24_BITS;  // 地址大小 24 位
  sCommand.Address = 0;   // 地址值
  sCommand.DataMode = QSPI_DATA_4_LINES;  // ⭐ 四线数据模式（关键区别）      ------
  sCommand.DummyCycles = 0;  // 空闲周期 0
  sCommand.NbData = length;  // 数据长度
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_1_LINE;  // 单线备用字节模式      ------
  sCommand.AlternateBytes = 0x003c00;   // ⭐ 备用字节（关键区别）     ------
  sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_24_BITS;  // 备用字节大小 24 位    ------
  sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;   // 禁用 DDR 模式
  sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR 保持半周期
  sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;   // 每次命令都发送指令

  // 这个备用字节的作用：
  // 1. 在指令和数据之间插入额外的控制信息
  // 2. 可能用于 OLED 显示屏的特殊命令格式
  // 3. 0x003c00 可能是 OLED 厂商定义的特定命令前缀  
  // 先发指令再发备用指令 再发数据
  // 发鿁写入命仿
  if (HAL_QSPI_Command(&hqspi, &sCommand, 1000) != HAL_OK)
  {
    Error_Handler();
  }

  // 发鿁数捿
  if (HAL_QSPI_Transmit_DMA(&hqspi, date) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 1 */
// 用硬件抽象层包装hal库发送数�?
void qspi_send_message1(uint8_t *date, uint16_t length)
{
  QSPI_CommandTypeDef sCommand;
  sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction = 0x32; // 写命�?
  sCommand.AddressMode = QSPI_ADDRESS_NONE;
  sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
  sCommand.Address = 0;
  sCommand.DataMode = QSPI_DATA_4_LINES;
  sCommand.DummyCycles = 0;
  sCommand.NbData = length;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_1_LINE;
  sCommand.AlternateBytes = 0x002c00;
  sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_24_BITS;
  sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  // 发鿁写入命仿
  if (HAL_QSPI_Command(&hqspi, &sCommand, 1000) != HAL_OK)
  {
    Error_Handler();
  }

  // 发鿁数捿
  if (HAL_QSPI_Transmit_DMA(&hqspi, date) != HAL_OK)
  {
    Error_Handler();
  }
}
/**
 * @brief  Tx Transfer completed callback.
 * @param  hqspi QSPI handle
 * @retval None
 */
void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
  /* Prevent unused argument(s) compilation warning */
  qspi_tx_done = 1;

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_QSPI_TxCpltCallback could be implemented in the user file
   */
}
/* USER CODE END 1 */

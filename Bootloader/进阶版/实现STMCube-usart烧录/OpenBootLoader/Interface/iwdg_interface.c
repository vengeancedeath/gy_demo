/**
  ******************************************************************************
  * @file    iwdg_interface.c
  * @author  MCD Application Team
  * @brief   Contains IWDG HW configuration
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
#include "main.h"
#include "iwdg_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static IWDG_HandleTypeDef IWDGHandle;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  配置 IWDG 独立看门狗
  *
  *         用户可能通过选项字节将 IWDG 设为硬件启动模式（IWDG_SW=0），
  *         芯片上电后 IWDG 即开始运行，Bootloader 必须立即重新配置并喂狗，
  *         否则计数器归零将触发系统复位。
  *
  *         采用直接寄存器操作而非 HAL_IWDG_Init()，避免等待 SR 标志清零
  *         期间计数器归零导致复位。
  *
  *         超时时间：Prescaler=256, Reload=0xFFF, LSI≈40kHz → 约 26.2 秒
  *
  * @retval 无
  */
void OPENBL_IWDG_Configuration(void)
{
  IWDGHandle.Instance       = IWDG;
  IWDGHandle.Init.Prescaler = IWDG_PRESCALER_256;
  IWDGHandle.Init.Window    = IWDG_WINDOW_DISABLE;
  IWDGHandle.Init.Reload    = 0xFFF;

  /* 解锁 PR/RLR/WINR 寄存器写访问 */
  IWDG->KR = IWDG_KEY_WRITE_ACCESS_ENABLE;

  /* 预分频 256，LSI 40kHz/256 ≈ 156Hz */
  IWDG->PR = IWDG_PRESCALER_256;

  /* 重载值 0xFFF=4096，超时 ≈ 26.2s */
  IWDG->RLR = 0xFFF;

  /* 禁用窗口模式，允许任意时刻喂狗 */
  IWDG->WINR = IWDG_WINDOW_DISABLE;

  /* 立即喂狗，将 RLR 值加载到计数器 */
  IWDG->KR = IWDG_KEY_RELOAD;
}

/**
  * @brief  刷新 IWDG 计数器（喂狗）
  *
  *         IWDG 一旦启动便无法停止，计数器递减到 0 将触发系统复位。
  *         向 KR 写入 IWDG_KEY_RELOAD(0xAAAA)，硬件将 RLR 值重载到计数器。
  *
  * @retval 无
  *
  * @note   通信循环中应周期性调用，Flash 擦除等耗时操作前后必须喂狗
  */
void OPENBL_IWDG_Refresh(void)
{
  if (HAL_IWDG_Refresh(&IWDGHandle) != HAL_OK)
  {
    Error_Handler();
  }
}
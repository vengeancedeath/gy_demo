/**
  ******************************************************************************
  * @file    openbootloader_conf.h
  * @author  MCD Application Team
  * @brief   Contains Open Bootloader configuration
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OPENBOOTLOADER_CONF_H
#define OPENBOOTLOADER_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "platform.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* -------------------------------- Device ID ------------------------------- */
#define DEVICE_ID_MSB                     0x04              /* MSB byte of device ID (STM32F072 = 0x0447) */
#define DEVICE_ID_LSB                     0x48              /* LSB byte of device ID */

/* -------------------------- Definitions for Memories ---------------------- */
#define FLASH_START_ADDRESS               0x08000000UL
#define FLASH_END_ADDRESS                 (FLASH_START_ADDRESS + (128 * 1024))

#define RAM_START_ADDRESS                 0x20000000UL
#define RAM_END_ADDRESS                   (RAM_START_ADDRESS + (16 * 1024))

#define OB_START_ADDRESS                  0x1FFFF800U                /* Option bytes & system area start (CubeProgrammer compatible) */
#define OB_END_ADDRESS                    0x1FFFFF80U                /* Option bytes & system area end */

// #define OTP_START_ADDRESS                 0x1FFFF700U                   /* OTP registers address */
// #define OTP_END_ADDRESS                   0x1FFFF7FFU                   /* OTP end address */

// #define ICP1_START_ADDRESS                0x1FFFF000U                  /* System memory registers address */
// #define ICP1_END_ADDRESS                  0x1FFFF6FFU                  /* System memory registers end address */

// #define ICP2_START_ADDRESS                0x1FFFF800U                  /* System memory registers address */
// #define ICP2_END_ADDRESS                  0x1FFFFFFFU                  /* System memory registers end address */

// #define EB_START_ADDRESS                  0x1FFFF750U                /* engi bytes start address */
// #define EB_END_ADDRESS                    0x1FFFF77FU                /* engi bytes end address  */

#define OPENBL_RAM_SIZE                   0x00003000U               /* RAM used by the Open Bootloader */

#define APP_VECTOR_NUM                    48U                       /* 16 system exceptions + 32 external interrupts (STM32F072) */

#define RDP_LEVEL_0                       OB_RDP_LEVEL_0
#define RDP_LEVEL_1                       OB_RDP_LEVEL_1
#define RDP_LEVEL_2                       OB_RDP_LEVEL_2

#define AREA_ERROR                        0x0U              /* Error Address Area */
#define FLASH_AREA                        0x1U              /* Flash Address Area */
#define RAM_AREA                          0x2U              /* RAM Address area */
#define OB_AREA                           0x3U              /* Option bytes Address area */
#define OTP_AREA                          0x4U              /* OTP Address area */
#define ICP1_AREA                         0x5U              /* System memory area */
#define ICP2_AREA                         0x6U              /* System memory area */
#define EB_AREA                           0x7U              /* Engi bytes Address area */

#define FLASH_MASS_ERASE                  0xFFFF
#define FLASH_BANK1_ERASE                 0xFFFE
#define FLASH_BANK2_ERASE                 0xFFFD

#define INTERFACES_SUPPORTED              2U

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* OPENBOOTLOADER_CONF_H */
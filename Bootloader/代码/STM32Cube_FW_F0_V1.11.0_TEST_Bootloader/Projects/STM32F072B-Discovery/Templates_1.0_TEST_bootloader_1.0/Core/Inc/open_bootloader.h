#ifndef __OPEN_BOOTLOADER_H__
#define __OPEN_BOOTLOADER_H__

#include <stdint.h>
#include "main.h"
#include "logger.h"
#include  <string.h>


#define bootloader_st "start bootloader"
#define APP_START_ADDR  0x0800A000UL
#define APP_FLASH_SIZE  0xA000UL
#define APP_END_ADDR    (APP_START_ADDR + APP_FLASH_SIZE)

#define SRAM_BASE  0x20000000UL
#define SRAM_SIZE  0x4000UL
#define SRAM_END   (SRAM_BASE + SRAM_SIZE)

#define APP_VECTOR_NUM  48U   /* 16系统异常 + 32外部中断(STM32F072) */

void jump_to_app(void);





#endif // __OPEN_BOOTLOADER_H__

#include "open_bootloader.h"

static  const char *log_tag = "main";
#define BT_BUSS_INFO(format, ...) LOG_INFO(log_tag, format, ##__VA_ARGS__)

//NOTE:
//APP程序一开始需要调用 __enable_irq();



/**
  * @brief  跳转到应用程序（App）
  *         从Bootloader切换到App执行环境，流程：
  *         1. 读取App向量表的初始MSP和复位向量
  *         2. 校验MSP位于SRAM范围且4字节对齐
  *         3. 校验复位向量位于App Flash范围（清除Thumb位后判断）
  *         4. 反初始化HAL层、关闭所有中断、清除挂起中断、关闭SysTick
  *         5. 将App向量表拷贝到SRAM并重映射SRAM到0x00000000（Cortex-M0无VTOR）
  *         6. 通过汇编设置MSP并跳转到App复位函数
  * @note   跳转后不会返回；若校验失败则打印错误并返回
  * @param  无
  * @retval 无

Note:
Cortex-M0	❌ 无VTOR	固定在 0x00000000，需通过 SYSCFG 内存重映射
Cortex-M0+ / M3 / M4 / M7	✅ 有	直接写 SCB->VTOR


APP程序的内存设置
IROM1 Start	0x0800A000	正常修改即可
IROM1 Size	0xA000    正常修改即可

通过 SYSCFG 内存重映射
IRAM1需要修改
向量表大小 = 48 向量 × 4 字节 = 192 字节 = 0xC0
IRAM1 Start = 0x20000000 + 0xC0 = 0x200000C0
IRAM1 Size  = 0x4000 - 0xC0    = 0x3F40
验证: 0x200000C0 + 0x3F40 = 0x20004000 = SRAM 结束地址 ✓



  */
void jump_to_app(void)
{
	/* 读取App向量表：第1个字为初始栈指针(MSP)，第2个字为复位向量地址 */
	uint32_t app_msp = *(__IO uint32_t *)APP_START_ADDR;
	uint32_t app_reset_addr = *(__IO uint32_t *)(APP_START_ADDR + 4UL);

	/* 校验MSP：须落在SRAM范围内(0x20000000~0x20003FFF)且4字节对齐 */
	if((app_msp < SRAM_BASE) || (app_msp > SRAM_END) || ((app_msp & 0x3UL) != 0UL))
	{
		BT_BUSS_INFO("APP MSP ERROR = 0x%08X", app_msp);
		return;
	}

	/* 校验复位向量：清除Thumb标志位(LSB)后判断是否落在App Flash范围 */
	uint32_t reset_func_addr = app_reset_addr & ~1UL;
	if((reset_func_addr < APP_START_ADDR) || (reset_func_addr > APP_END_ADDR))
	{
		BT_BUSS_INFO("APP RESET ADDR ERROR = 0x%08X", app_reset_addr);
		return;
	}

  
	/* 反初始化HAL层（含外设复位、SysTick停用、时钟复位），恢复上电默认状态 */
	HAL_DeInit();

	/* 关闭所有外部中断并清除挂起标志（Cortex-M0仅ICER[0]/ICPR[0]有效） */
	NVIC->ICER[0] = 0xFFFFFFFFUL;
	NVIC->ICPR[0] = 0xFFFFFFFFUL;

	/* 关闭SysTick定时器，避免其周期中断干扰App启动 */
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	/* 关闭全局中断，保证跳转过程原子性 */
	__disable_irq();

	/* Cortex-M0无VTOR寄存器：将App向量表拷贝到SRAM起始，再重映射SRAM到0x00000000 */
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	for(uint32_t i = 0; i < APP_VECTOR_NUM; i++)
	{
		*((__IO uint32_t *)(SRAM_BASE + (i << 2))) = *((__IO uint32_t *)(APP_START_ADDR + (i << 2)));
	}
	__HAL_SYSCFG_REMAPMEMORY_SRAM();

	/* 内存屏障：确保SRAM重映射完全生效后再执行跳转 */
	__DSB();
	__ISB();

	/* 设置App主栈指针并跳转到复位函数（汇编实现，避免中间栈访问风险） */
	__asm volatile
	(
		"MSR MSP, %0\n"
		"BX %1\n"
		:
		: "r"(app_msp), "r"(app_reset_addr)
		: "memory"
	);
// typedef void (*app_reset_handler_t)(void);
//app_reset_handler_t app_reset_handle = (app_reset_handler_t)app_reset_addr;
//app_reset_handle();

	/* 跳转失败容错：正常不会执行到这里 */
	while(1)
  {
    BT_BUSS_INFO("jump_to_app error");

  }
}

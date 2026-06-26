/**
  ******************************************************************************
  * @file    flash_interface.c
  * @author  MCD Application Team
  * @brief   Contains FLASH access functions
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
#include "openbl_mem.h"
#include "app_openbootloader.h"
#include "common_interface.h"
#include "flash_interface.h"

/* Private typedef -----------------------------------------------------------*/

/**
  * @brief  忙碌字节发送回调函数指针
  *         在 Flash 擦除/编程等耗时操作期间，通过此回调向主机发送"忙碌"状态字节，
  *         防止主机因超时而断开连接。主要用于 I2C 非拉伸模式下通知主机从设备正忙。
  *         初始值为 0（未注册回调）。
  */
// Send_BusyByte_Func *BusyByteCallback = 0;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief  Flash 忙碌状态标志
  *         - FLASH_BUSY_STATE_ENABLED  (0xAAAA0000)：使能忙碌状态发送，
  *           擦除/编程期间会通过 BusyByteCallback 回调发送忙碌字节
  *         - FLASH_BUSY_STATE_DISABLED (0x0000DDDD)：禁用忙碌状态发送
  *         默认为 DISABLED，仅在需要时通过 OPENBL_Enable_BusyState_Sending() 使能
  */
// uint32_t Flash_BusyState = FLASH_BUSY_STATE_DISABLED;

/* Private function prototypes -----------------------------------------------*/
static void OPENBL_FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data);
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *ListOfPages, uint32_t Length);
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void);

/* Exported variables --------------------------------------------------------*/

/**
  * @brief  Flash 存储器描述符
  *         定义了 Flash 的地址范围、容量、类型以及各种操作回调函数。
  *         在 OpenBootloader_Init() 中通过 OPENBL_MEM_RegisterMemory() 注册到内存管理模块，
  *         之后命令处理层通过统一接口（OPENBL_MEM_Read/Write/Erase 等）间接调用这些回调。
  *
  *         字段说明：
  *         - StartAddress : Flash 起始地址 = FLASH_BASE = 0x08000000
  *         - EndAddress   : Flash 结束地址 = FLASH_BASE + 128KB = 0x0801FFFF
  *         - Size         : Flash 总容量 = 128KB（STM32F072CBT6）
  *         - Type         : 存储区类型 = FLASH_AREA (0x1)
  *         - Read         : 读操作回调 → OPENBL_FLASH_Read
  *         - Write        : 写操作回调 → OPENBL_FLASH_Write
  *         - SetReadoutProtect : 读保护设置回调 → OPENBL_FLASH_SetReadOutProtectionLevel
  *         - SetWriteProtect    : 写保护设置回调 → OPENBL_FLASH_SetWriteProtection
  *         - JumpToAddress      : 跳转回调 → OPENBL_FLASH_JumpToAddress
  *         - MassErase          : 批量擦除回调 → NULL（使用 Erase 统一处理）
  *         - Erase              : 页擦除回调 → OPENBL_FLASH_Erase
  */
OPENBL_MemoryTypeDef FLASH_Descriptor =
{
  FLASH_START_ADDRESS,
  FLASH_END_ADDRESS,
  (128U * 1024U),
  FLASH_AREA,
  OPENBL_FLASH_Read,
  OPENBL_FLASH_Write,
  OPENBL_FLASH_SetReadOutProtectionLevel,
  OPENBL_FLASH_SetWriteProtection,
  OPENBL_FLASH_JumpToAddress,
  NULL,
  OPENBL_FLASH_Erase
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  解锁 Flash 控制寄存器访问
  *         STM32 的 Flash 控制寄存器（FLASH_CR）默认是锁定的，防止误操作。
  *         在执行编程、擦除等修改操作前必须先解锁。
  *         解锁机制：向 FLASH_KEYR 寄存器依次写入 KEY1=0x45670123 和 KEY2=0xCDEF89AB。
  *
  * @note   必须在所有 Flash 写操作（编程/擦除/选项字节）之前调用
  * @retval 无
  */
void OPENBL_FLASH_Unlock(void)
{
  HAL_FLASH_Unlock();
}

/**
  * @brief  锁定 Flash 控制寄存器访问
  *         完成 Flash 操作后应立即锁定，防止后续代码意外修改 Flash 内容。
  *         锁定后 FLASH_CR 的 PG（编程）和 PER（页擦除）等位将无法置位。
  *
  * @note   必须在所有 Flash 写操作完成之后调用
  * @retval 无
  */
void OPENBL_FLASH_Lock(void)
{
  HAL_FLASH_Lock();
}

/**
  * @brief  解锁 Flash 选项字节寄存器访问
  *         选项字节（Option Bytes）控制 Flash 的读保护、写保护、看门狗配置等。
  *         修改选项字节需要双重解锁：先解锁 Flash 主锁，再解锁选项字节锁。
  *         选项字节解锁机制：向 FLASH_OPTKEYR 寄存器依次写入 OPTKEY1 和 OPTKEY2。
  *
  * @note   在修改 RDP/WRP 等选项字节之前必须调用此函数
  * @retval 无
  */
void OPENBL_FLASH_OB_Unlock(void)
{
  /* 先解锁 Flash 主锁，否则选项字节锁无法解锁 */
  HAL_FLASH_Unlock();

  /* 再解锁选项字节锁 */
  HAL_FLASH_OB_Unlock();
}

/**
  * @brief  启动选项字节加载（OB Launch）
  *         修改选项字节后，新的值存储在选项字节寄存器中，但不会立即生效。
  *         调用此函数会触发 OBL (Option Byte Load) 操作，将新值加载到 Flash 控制器中。
  *         OBL 操作会引发系统复位，复位后新的选项字节配置才会生效。
  *
  * @warning 调用此函数后系统会自动复位！
  * @retval 无（函数不会返回，因为系统会复位）
  */
void OPENBL_FLASH_OB_Launch(void)
{
  HAL_FLASH_OB_Launch();
}

/**
  * @brief  从指定地址读取一个字节
  *         直接通过指针解引用读取 Flash 内容，因为 STM32 的 Flash 是内存映射的，
  *         可以像访问 RAM 一样直接通过地址读取。
  *
  * @param  Address 要读取的 Flash 地址，必须在 FLASH_BASE ~ FLASH_BANK1_END 范围内
  * @retval 读取到的字节数据（0x00 ~ 0xFF）
  *
  * @note   此函数不做地址合法性检查，调用者需确保地址有效
  */
uint8_t OPENBL_FLASH_Read(uint32_t Address)
{
  return (*(volatile uint8_t *)(Address));
}

/**
  * @brief  向 Flash 写入数据
  *         将指定长度的数据写入 Flash 的指定地址。
  *         STM32F072 支持 DoubleWord（64位）编程，因此数据长度会向上对齐到 8 字节。
  *
  *         执行流程：
  *         1. 将数据长度向上对齐到 8 字节（Flash 编程最小单位为 DoubleWord）
  *         2. 解锁 Flash
  *         3. 循环调用 OPENBL_FLASH_ProgramDoubleWord() 逐个写入 64 位数据
  *         4. 锁定 Flash
  *
  * @param  Address     写入起始地址，必须是 8 字节对齐（DoubleWord 对齐）
  * @param  Data       指向待写入数据的指针
  * @param  DataLength 待写入数据的字节长度
  * @retval 无
  *
  * @note   写入前必须确保目标地址已被擦除（Flash 只能从 1 写到 0，不能反向）
  * @note   如果 DataLength 不是 8 的整数倍，多余的字节会被写入未定义数据
  */
void OPENBL_FLASH_Write(uint32_t Address, uint8_t *Data, uint32_t DataLength)
{
  uint32_t index  = 0U;
  uint32_t length = DataLength;

  /* 将长度向上对齐到 8 字节（DoubleWord 编程粒度）
   * 例如：DataLength=5 → length=8, DataLength=16 → length=16
   * 原理：如果最低3位不为0，则清零最低3位后加8 */
  if (length & 7U)
  {
    length = (length & 0xFFFFFFF8U) + 8U;
  }

  /* 解锁 Flash 以允许编程操作 */
  OPENBL_FLASH_Unlock();

  /* 逐个 DoubleWord（8字节）写入
   * 每次取 Data 缓冲区中偏移 index 处的 8 字节，作为 uint64_t 写入 Flash */
  for (index = 0U; index < length; (index += 8U))
  {
    OPENBL_FLASH_ProgramDoubleWord((Address + index), (uint64_t)(*((uint64_t *)((uint32_t)Data + index))));
  }

  /* 编程完成后锁定 Flash，防止误操作 */
  OPENBL_FLASH_Lock();
}

/**
  * @brief  跳转到指定地址执行用户应用程序
  *         这是 Bootloader 的核心功能之一：在收到 GO 命令后，
  *         将 CPU控制权从 Bootloader 转交给用户应用程序。
  *
  *         执行流程：
  *         1. 反初始化 Bootloader 使用的硬件资源（恢复复位值）
  *         2. 使能全局中断（Bootloader 运行期间可能已禁用）
  *         3. 从 Address+4 处读取用户应用的 Reset_Handler 地址（即中断向量表第2项）
  *         4. 从 Address 处读取用户应用的栈顶地址（即中断向量表第1项），设置 MSP
  *         5. 跳转到 Reset_Handler 执行
  *
  *         ARM Cortex-M0 中断向量表结构：
  *         Address+0x00: 初始栈指针值（MSP）
  *         Address+0x04: Reset_Handler 入口地址
  *         Address+0x08: NMI_Handler
  *         ...
  *
  * @param  Address 用户应用程序的起始地址（通常为 0x08000000 或其他 Flash 区域）
  * @retval 无（函数不会返回，因为 PC 指针已跳转到用户应用）
  *
  * @warning 调用前必须确保 Address 处有有效的中断向量表
  */
void OPENBL_FLASH_JumpToAddress(uint32_t Address)
{
  Function_Pointer jump_to_address;
  uint32_t i;

  /* 反初始化 Bootloader 使用的外设，恢复为复位默认值 */
  OpenBootloader_DeInit();

  /* 关闭所有外部中断并清除挂起标志 */
  NVIC->ICER[0] = 0xFFFFFFFFUL;
  NVIC->ICPR[0] = 0xFFFFFFFFUL;

  /* 关闭 SysTick 定时器 */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL  = 0;

  /* 关闭全局中断，保证跳转过程原子性 */
  Common_DisableIrq();

  /* Cortex-M0 无 VTOR 寄存器：将 App 向量表拷贝到 SRAM 起始，
   * 再通过 SYSCFG 重映射 SRAM 到 0x00000000，
   * 使中断向量表指向 App 而非 Bootloader */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  for (i = 0U; i < APP_VECTOR_NUM; i++)
  {
    *((__IO uint32_t *)(SRAM_BASE + (i << 2))) = *((__IO uint32_t *)(Address + (i << 2)));
  }
  __HAL_SYSCFG_REMAPMEMORY_SRAM();

  /* 内存屏障：确保 SRAM 重映射完全生效后再执行跳转 */
  __DSB();
  __ISB();

  /* 从向量表偏移 +0 处读取初始栈指针值并设置 MSP */
  Common_SetMsp(*(__IO uint32_t *) Address);

  /* 从向量表偏移 +4 处读取 Reset_Handler 的地址 */
  jump_to_address = (Function_Pointer)(*(__IO uint32_t *)(Address + 4U));

  /* 跳转到用户应用的 Reset_Handler 执行 */
  jump_to_address();
}

/**
  * @brief  获取当前 Flash 读保护（RDP）等级
  *         通过读取选项字节寄存器获取当前的读保护配置。
  *
  *         RDP 等级说明：
  *         - OB_RDP_LEVEL_0 (0xAA)：无保护，任何调试器都可以读取 Flash 内容
  *         - OB_RDP_LEVEL_1 (0xBB)：读保护，调试器无法读取 Flash 内容，
  *           但可以从 Flash 执行代码。从 Level 1 降级到 Level 0 会触发全片擦除
  *         - OB_RDP_LEVEL_2 (0xCC)：永久读保护，不可逆！
  *           调试器连接被永久禁止，选项字节也无法再修改
  *
  * @retval 当前 RDP 等级（OB_RDP_LEVEL_0 / OB_RDP_LEVEL_1 / OB_RDP_LEVEL_2）
  */
uint32_t OPENBL_FLASH_GetReadOutProtectionLevel(void)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  /* 读取当前选项字节配置 */
  HAL_FLASHEx_OBGetConfig(&flash_ob);

  return flash_ob.RDPLevel;
}

/**
  * @brief  设置 Flash 读保护（RDP）等级
  *         修改选项字节中的 RDP 字段来改变读保护等级。
  *
  *         安全机制：
  *         - 禁止设置 Level 2（0xCC），因为这是不可逆操作，一旦设置将永久锁定芯片
  *         - 从 Level 1 降到 Level 0 时，硬件会自动执行全片擦除，防止通过降级来读取受保护的数据
  *
  * @param  Level 目标 RDP 等级：
  *         - OB_RDP_LEVEL_0：取消读保护（如果当前是 Level 1，会触发全片擦除）
  *         - OB_RDP_LEVEL_1：启用读保护
  *         - OB_RDP_LEVEL_2：永久读保护（此函数会拒绝此值，不执行任何操作）
  * @retval 无
  *
  * @note   修改后需要调用 OPENBL_FLASH_OB_Launch() 使新配置生效（会触发系统复位）
  */
void OPENBL_FLASH_SetReadOutProtectionLevel(uint32_t Level)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  /* 安全检查：禁止设置 RDP Level 2
   * Level 2 是不可逆的永久保护，一旦设置芯片将永久无法调试
   * Bootloader 不应提供此功能，防止误操作导致芯片变"砖" */
  if (Level != OB_RDP_LEVEL_2)
  {
    flash_ob.OptionType = OPTIONBYTE_RDP;
    flash_ob.RDPLevel   = Level;

    /* 解锁 Flash 和选项字节寄存器 */
    OPENBL_FLASH_OB_Unlock();

    /* 写入新的 RDP 等级到选项字节 */
    HAL_FLASHEx_OBProgram(&flash_ob);
  }
}

/**
  * @brief  设置或取消 Flash 写保护
  *         写保护可以防止指定 Flash 页被意外擦除或编程。
  *         此函数是写保护的总入口，根据 State 参数分发到使能或禁用函数。
  *
  * @param  State        操作类型：
  *         - ENABLE  ：启用写保护（调用 OPENBL_FLASH_EnableWriteProtection）
  *         - DISABLE ：禁用写保护（调用 OPENBL_FLASH_DisableWriteProtection）
  * @param  ListOfPages 页面列表缓冲区指针，格式取决于具体操作：
  *         - 使能时：包含要保护的页掩码数据
  *         - 禁用时：此参数被忽略
  * @param  Length      页面列表的长度（字节数）
  * @retval ErrorStatus 操作结果：
  *         - SUCCESS：操作成功
  *         - ERROR  ：操作失败或参数无效（如 State 不是 ENABLE/DISABLE）
  */
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState State, uint8_t *ListOfPages, uint32_t Length)
{
  ErrorStatus status = SUCCESS;

  if (State == ENABLE)
  {
    /* 使能写保护：根据 ListOfPages 指定的页设置保护 */
    OPENBL_FLASH_EnableWriteProtection(ListOfPages, Length);
  }
  else if (State == DISABLE)
  {
    /* 禁用写保护：取消所有页的写保护 */
    OPENBL_FLASH_DisableWriteProtection();
  }
  else
  {
    /* 无效的 State 参数 */
    status = ERROR;
  }

  return status;
}

/**   目前无调用，保留接口
  * @brief  执行 Flash 全片擦除（Mass Erase）
  *         一次性擦除整个 Flash 的所有页，将所有内容恢复为 0xFF。
  *
  *         STM32F072CBT6 是单 Bank 架构，全片擦除会清除 128KB（64页）的所有数据。
  *         协议兼容性：虽然 F072 没有双 Bank，但仍接受 FLASH_BANK1_ERASE 命令
  *         （等同于全片擦除），以保持与上位机协议的兼容。
  *
  *         数据格式（p_Data 缓冲区）：
  *         - 前 2 字节为擦除选项（小端序 uint16_t）：
  *           0xFFFF (FLASH_MASS_ERASE)  ：全片擦除
  *           0xFFFE (FLASH_BANK1_ERASE) ：Bank1 擦除（F072 上等同于全片擦除）
  *           0xFFFD (FLASH_BANK2_ERASE) ：Bank2 擦除（F072 无 Bank2，返回 ERROR）
  *
  * @param  p_Data     指向擦除选项数据的指针
  * @param  DataLength 数据长度（字节），必须 >= 2
  * @retval ErrorStatus 操作结果：
  *         - SUCCESS：擦除成功
  *         - ERROR  ：擦除失败、数据长度不足或无效的擦除选项
  */
ErrorStatus OPENBL_FLASH_MassErase(uint8_t *p_Data, uint32_t DataLength)
{
  uint32_t page_error  = 0U;
  uint16_t bank_option = 0U;
  ErrorStatus status   = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* 解锁 Flash 以允许擦除操作 */
  OPENBL_FLASH_Unlock();

  /* 配置擦除参数：全片擦除
   * STM32F0 的擦除结构体使用 PageAddress + NbPages 方式
   * PageAddress 设为 FLASH_BASE 表示从 Flash 起始地址开始
   * NbPages 设为 1（全片擦除模式下此字段被 HAL 忽略） */
  erase_init_struct.TypeErase    = FLASH_TYPEERASE_MASSERASE;
  erase_init_struct.PageAddress  = FLASH_BASE;
  erase_init_struct.NbPages      = 1U;

  if (DataLength >= 2)
  {
    /* 从数据缓冲区读取擦除选项（小端序 16 位） */
    bank_option = *(uint16_t *)(p_Data);

    /* F072 是单 Bank 架构：
     * - FLASH_MASS_ERASE (0xFFFF)：全片擦除 → 执行
     * - FLASH_BANK1_ERASE (0xFFFE)：Bank1 擦除 → 等同全片擦除，执行
     * - FLASH_BANK2_ERASE (0xFFFD)：Bank2 擦除 → F072 无 Bank2，返回错误 */
    if ((bank_option == FLASH_MASS_ERASE) || (bank_option == FLASH_BANK1_ERASE))
    {
      if (HAL_FLASHEx_Erase(&erase_init_struct, &page_error) != HAL_OK)
      {
        status = ERROR;
      }
      else
      {
        status = SUCCESS;
      }
    }
    else
    {
      /* 无效的擦除选项（如 Bank2 擦除） */
      status = ERROR;
    }
  }
  else
  {
    /* 数据长度不足，无法读取擦除选项 */
    status = ERROR;
  }

  /* 擦除完成后锁定 Flash */
  OPENBL_FLASH_Lock();

  return status;
}

/**
  * @brief  执行 Flash 页擦除操作
  *         按页擦除指定的 Flash 区域，每页 2KB（FLASH_PAGE_SIZE = 0x800）。
  *         STM32F072CBT6 共有 64 页（页号 0~63），总计 128KB。
  *
  *         数据格式（p_Data 缓冲区，所有字段为小端序 uint16_t）：
  *         ┌──────────┬──────────┬──────────┬──────────┬─────┐
  *         │ 页数量 N  │ 页号 0   │ 页号 1   │ 页号 2   │ ... │
  *         │ (2字节)  │ (2字节)  │ (2字节)  │ (2字节)  │     │
  *         └──────────┴──────────┴──────────┴──────────┴─────┘
  *         - 第1个 uint16_t：要擦除的页总数 N
  *         - 后续 N 个 uint16_t：要擦除的页号（0~63）
  *
  *         地址计算方式：
  *         页地址 = FLASH_BASE + (页号 × FLASH_PAGE_SIZE)
  *         例如：页号 0 → 0x08000000，页号 1 → 0x08000800，页号 63 → 0x0801F800
  *
  * @param  p_Data     指向擦除命令数据的指针
  * @param  DataLength 数据总长度（字节），用于防止越界读取
  * @retval ErrorStatus 操作结果：
  *         - SUCCESS：所有页擦除成功
  *         - ERROR  ：至少有一页擦除失败
  */
ErrorStatus OPENBL_FLASH_Erase(uint8_t *p_Data, uint32_t DataLength)
{
  uint32_t counter      = 0U;
  uint32_t pages_number = 0U;
  uint32_t page_error   = 0U;
  uint32_t errors       = 0U;
  uint32_t page_index   = 0U;
  ErrorStatus status    = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* 解锁 Flash 以允许擦除操作 */
  OPENBL_FLASH_Unlock();

  /* 清除 Flash 错误标志位
   * STM32F0 使用 FLASH_FLAG_PGERR（编程错误）和 FLASH_FLAG_WRPERR（写保护错误）
   * 必须在擦除前清除，否则上次操作的残留标志会导致 HAL_FLASHEx_Erase 返回错误 */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

  /* 读取要擦除的页数量（p_Data 的前 2 字节，小端序 uint16_t） */
  pages_number  = (uint32_t)(*(uint16_t *)(p_Data));
  /* 跳过页数量字段，指向第一个页号 */
  p_Data       += 2;

  /* 配置擦除参数：按页擦除，每次擦除 1 页 */
  erase_init_struct.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_init_struct.NbPages   = 1U;

  /* 逐页擦除
   * 循环条件：未达到请求的页数 且 未超出数据缓冲区长度
   * DataLength/2 是 p_Data 中可用的 uint16_t 元素个数（含页数量字段） */
  for (counter = 0U; ((counter < pages_number) && (counter < (DataLength / 2U))) ; counter++)
  {
    /* 读取当前要擦除的页号 */
    page_index = ((uint32_t)(*(uint16_t *)(p_Data)));

    /* STM32F072CBT6 有效页号为 0~63（128KB / 2KB 每页 = 64 页）
     * 超出范围的页号直接跳过，不视为错误 */
    if (page_index <= 63U)
    {
      /* 将页号转换为绝对地址
       * 例如：页号 0 → 0x08000000, 页号 1 → 0x08000800 */
      erase_init_struct.PageAddress = FLASH_BASE + (page_index * FLASH_PAGE_SIZE);

      /* 执行单页擦除，如果失败则累计错误计数 */
      if (HAL_FLASHEx_Erase(&erase_init_struct, &page_error) != HAL_OK)
      {
        errors++;
      }
    }

    /* 移动到下一个页号 */
    p_Data += 2;
  }

  /* 检查是否有擦除失败的页 */
  if (errors > 0)
  {
    status = ERROR;
  }
  else
  {
    /* 所有页擦除成功，锁定 Flash */
    OPENBL_FLASH_Lock();
    status = SUCCESS;
  }

  return status;
}


/**
  * @brief  使能 Flash 操作期间的忙碌状态发送
  *         在 I2C 非时钟拉伸模式下，从设备无法通过拉伸时钟来通知主机"正忙"，
  *         因此需要在 Flash 擦除/编程期间主动发送一个"忙碌字节"（NACK）。
  *
  *         调用此函数后，Flash 操作期间会通过 pCallback 回调发送忙碌字节，
  *         让主机知道从设备正在处理，需要等待。
  *
  * @param  pCallback 忙碌字节发送回调函数指针，由通信接口（如 I2C）提供
  * @retval 无
  */
void OPENBL_Enable_BusyState_Sending(Send_BusyByte_Func *pCallback)
{
  // /* 使能 Flash 忙碌状态发送 */
  // Flash_BusyState = FLASH_BUSY_STATE_ENABLED;

  // /* 保存回调函数指针，Flash 操作代码会检查此指针并发送忙碌字节 */
  // BusyByteCallback = pCallback;
}

/**
  * @brief  禁用 Flash 操作期间的忙碌状态发送
  *         在 I2C 时钟拉伸模式或其他通信接口（如 USART）下，
  *         不需要发送忙碌字节，因为从设备可以通过拉伸时钟或协议流控来通知主机。
  *         调用此函数将停止忙碌字节的发送。
  *
  * @retval 无
  */
void OPENBL_Disable_BusyState_Sending(void)
{
  // /* 禁用 Flash 忙碌状态发送 */
  // Flash_BusyState = FLASH_BUSY_STATE_DISABLED;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  在指定 Flash 地址编程一个 DoubleWord（64位/8字节）
  *         这是 Flash 写操作的最底层实现，被 OPENBL_FLASH_Write() 调用。
  *
  *         STM32F072 支持三种编程粒度：
  *         - FLASH_TYPEPROGRAM_HALFWORD   (16位)
  *         - FLASH_TYPEPROGRAM_WORD       (32位)
  *         - FLASH_TYPEPROGRAM_DOUBLEWORD (64位) ← 本函数使用此模式
  *
  *         选择 DoubleWord 的原因：编程效率最高，每次写入 8 字节，
  *         减少解锁/锁定和等待完成的次数。
  *
  * @param  Address 编程目标地址，必须是 8 字节对齐（地址的 bit2~0 必须为 0）
  * @param  Data    要写入的 64 位数据
  * @retval 无
  *
  * @note   调用前必须确保：
  *         1. Flash 已解锁（HAL_FLASH_Unlock）
  *         2. 目标地址已被擦除（Flash 只支持从 1 写到 0）
  *         3. 地址在有效 Flash 范围内
  */
static void OPENBL_FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data)
{
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, Data);
}

/**
  * @brief  启用 Flash 写保护
  *         通过配置选项字节中的 WRP（Write Protection）字段，
  *         对指定的 Flash 页启用写保护，防止被意外擦除或编程。
  *
  *         STM32F0 的写保护机制采用位掩码方式：
  *         - WRPPage 是一个 32 位整数，每一位对应一组页（每 2 页一个 bit）
  *         - bit0 = OB_WRP_PAGES0TO1   ：保护第 0~1 页
  *         - bit1 = OB_WRP_PAGES2TO3   ：保护第 2~3 页
  *         - ...
  *         - bit31 = OB_WRP_PAGES62TO63：保护第 62~63 页
  *         - 0xFFFFFFFF = OB_WRP_ALLPAGES：保护所有页
  *
  *         数据格式（ListOfPages 缓冲区）：
  *         - ListOfPages[0] + ListOfPages[1]：第一组页掩码（低16位 + 高16位）
  *         - ListOfPages[2] + ListOfPages[3]：第二组页掩码（低16位 + 高16位）
  *         每组掩码可覆盖 32 个页保护位（对应 64 页），分两次写入可覆盖不同页组。
  *
  * @param  ListOfPages 页掩码数据缓冲区指针
  * @param  Length      缓冲区长度（字节数）：
  *         - Length >= 2：写入第一组页掩码
  *         - Length >= 4：写入第二组页掩码
  * @retval ErrorStatus 操作结果（当前实现始终返回 SUCCESS）
  *
  * @note   修改后需调用 OPENBL_FLASH_OB_Launch() 使配置生效
  */
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *ListOfPages, uint32_t Length)
{
  uint32_t wrp_pages     = 0U;
  ErrorStatus status     = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* 解锁 Flash 和选项字节寄存器 */
  OPENBL_FLASH_OB_Unlock();

  /* 配置选项字节类型为写保护，并设置为使能状态 */
  flash_ob.OptionType = OPTIONBYTE_WRP;
  flash_ob.WRPState   = OB_WRPSTATE_ENABLE;

  /* 第一组页掩码：ListOfPages[0] 为低字节，ListOfPages[1] 为高字节
   * 组合成 16 位掩码写入 WRPPage */
  if (Length >= 2)
  {
    wrp_pages = (uint32_t)(*(ListOfPages)) | ((uint32_t)(*(ListOfPages + 1)) << 8);

    flash_ob.WRPPage = wrp_pages;

    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  /* 第二组页掩码：ListOfPages[2] 为低字节，ListOfPages[3] 为高字节
   * 可用于保护另一组不同的页 */
  if (Length >= 4)
  {
    wrp_pages = (uint32_t)(*(ListOfPages + 2)) | ((uint32_t)(*(ListOfPages + 3)) << 8);

    flash_ob.WRPPage = wrp_pages;

    HAL_FLASHEx_OBProgram(&flash_ob);
  }

  return status;
}

/**
  * @brief  禁用 Flash 写保护
  *         取消所有 Flash 页的写保护，允许擦除和编程操作。
  *
  *         实现方式：将 WRPState 设为 DISABLE，WRPPage 设为 OB_WRP_ALLPAGES，
  *         一次性清除所有页的写保护。
  *
  *         注意：如果当前 RDP 等级为 Level 1，取消写保护后降级到 Level 0
  *         会触发全片擦除（硬件安全机制），防止通过取消保护来读取受保护数据。
  *
  * @retval ErrorStatus 操作结果（当前实现始终返回 SUCCESS）
  *
  * @note   修改后需调用 OPENBL_FLASH_OB_Launch() 使配置生效
  */
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void)
{
  ErrorStatus status       = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* 解锁 Flash 和选项字节寄存器 */
  OPENBL_FLASH_OB_Unlock();

  /* 配置选项字节：禁用写保护，并指定所有页（OB_WRP_ALLPAGES = 0xFFFFFFFF） */
  flash_ob.OptionType = OPTIONBYTE_WRP;
  flash_ob.WRPState   = OB_WRPSTATE_DISABLE;
  flash_ob.WRPPage    = OB_WRP_ALLPAGES;

  /* 写入选项字节，取消所有页的写保护 */
  HAL_FLASHEx_OBProgram(&flash_ob);

  return status;
}
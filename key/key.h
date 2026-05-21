/**
 * @file    key.h
 * @brief   独立按键驱动 - 支持短按、长按、双击检测
 * @note    可移植层: 需要用户实现 Key_Init() 和 Key_ReadPin()
 *          Key_Scan() 需放在 1ms 定时器中断中周期调用
 */

#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ======================== 移植配置宏 ======================== */
#define KEY_COUNT               4       /* 按键个数, 根据实际硬件修改 */
#define KEY_DEBOUNCE_MS         20      /* 消抖时间阈值 (ms), 1ms扫描一次 */
#define KEY_DEBOUNCE_TIMEOUT_MS 60      /* 消抖总超时 (ms), 防止状态机卡死, 建议 >= DEBOUNCE*3 */
#define KEY_LONG_PRESS_MS       1000    /* 长按触发阈值 (ms) */
#define KEY_DOUBLE_CLICK_MS     300     /* 双击间隔阈值 (ms) */
#define KEY_PRESS_LEVEL         0       /* 按键按下时的有效电平: 0=低有效, 1=高有效 */

/* ======================== 按键状态枚举 ======================== */
typedef enum {
    KEY_STATE_IDLE = 0,             /* 空闲, 等待按下 */
    KEY_STATE_DEBOUNCE_PRESS,       /* 按下消抖中 */
    KEY_STATE_PRESSED,              /* 已确认按下, 计时中 */
    KEY_STATE_DEBOUNCE_RELEASE,     /* 释放消抖中 */
    KEY_STATE_WAIT_DOUBLE_CLICK,    /* 等待第二次点击 (双击判定) */
} KeyState_t;

/* ======================== 按键事件枚举 ======================== */
typedef enum {
    KEY_EVENT_NONE = 0,             /* 无事件 */
    KEY_EVENT_SHORT_PRESS,          /* 短按事件 */
    KEY_EVENT_LONG_PRESS,           /* 长按事件 */
    KEY_EVENT_DOUBLE_CLICK,         /* 双击事件 */
} KeyEvent_t;

/* ======================== 按键结构体 ======================== */
typedef struct {
    volatile KeyState_t state;      /* 当前状态机状态 (ISR/主循环共享) */
    volatile KeyEvent_t  event;     /* 当前按键事件 (读取后自动清除) */
    volatile uint16_t    timer;     /* 通用计时器 (用于长按计时/双击间隔计时) */
    volatile uint16_t    debounce_count; /* 消抖计数器 */
    uint8_t     long_press_sent;    /* 长按事件已触发标志 */
    uint8_t     second_click;       /* 第二次点击标志 (双击检测用) */
} Key_t;


// 0    ----- GPIOA  GPIO_PIN_0
// 1    ----- GPIOA  GPIO_PIN_1
// 2    ----- GPIOA  GPIO_PIN_2
// 3    ----- GPIOA  GPIO_PIN_3

/* ======================== 外部变量声明 ======================== */
extern Key_t keys[KEY_COUNT];

/* ======================== 函数声明 ======================== */

/**
 * @brief  按键GPIO初始化 (移植层 - 需用户实现)
 * @note   初始化所有按键对应的GPIO为输入模式, 使能上拉/下拉
 */
void Key_Init(void);

/**
 * @brief  读取按键引脚电平 (移植层 - 需用户实现)
 * @param  key_id 按键索引, 范围 [0, KEY_COUNT)
 * @retval 1: 高电平, 0: 低电平
 */
uint8_t Key_ReadPin(uint8_t key_id);

/**
 * @brief  按键扫描函数 (在1ms定时器中断中调用)
 * @note   周期性调用此函数进行状态机扫描和消抖处理
 */
void Key_Scan(void);

/**
 * @brief  获取指定按键的事件 (在主循环中调用)
 * @param  key_id 按键索引, 范围 [0, KEY_COUNT)
 * @retval KeyEvent_t 按键事件 (读取后自动清除)
 */
KeyEvent_t Key_GetEvent(uint8_t key_id);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H */

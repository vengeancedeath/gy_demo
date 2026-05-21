/**
 * @file    key.c
 * @brief   独立按键驱动实现 - 支持短按、长按、双击检测
 * @note    Key_Scan() 放在 1ms 定时器中断中, Key_GetEvent() 在主循环中调用
 */

#include "key.h"

/* ======================== 按键结构体数组 ======================== */
Key_t keys[KEY_COUNT];

/**
 * @brief  按键GPIO初始化 (移植层 - 需用户根据硬件实现)
 * @note   示例: 将所有按键引脚配置为输入模式, 使能内部上拉电阻
 */
void Key_Init(void)
{
    /* TODO: 用户实现 - 初始化按键对应GPIO */
    /* 示例 (STM32 HAL):
     *   GPIO_InitTypeDef GPIO_InitStruct = {0};
     *   __HAL_RCC_GPIOx_CLK_ENABLE();
     *   GPIO_InitStruct.Pin   = GPIO_PIN_x;
     *   GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
     *   GPIO_InitStruct.Pull  = GPIO_PULLUP;
     *   HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
     */

    uint8_t i;
    for (i = 0; i < KEY_COUNT; i++) {
        keys[i].state            = KEY_STATE_IDLE;
        keys[i].event            = KEY_EVENT_NONE;
        keys[i].timer            = 0;
        keys[i].debounce_count   = 0;
        keys[i].long_press_sent  = 0;
        keys[i].second_click     = 0;
    }
}

/**
 * @brief  读取按键引脚电平 (移植层 - 需用户根据硬件实现)
 * @param  key_id 按键索引, 范围 [0, KEY_COUNT)
 * @retval 1: 高电平, 0: 低电平
 */
uint8_t Key_ReadPin(uint8_t key_id)
{
    /* TODO: 用户实现 - 根据key_id读取对应GPIO电平 */
    /* 示例 (STM32 HAL):
     *   switch (key_id) {
     *       case 0: return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
     *       case 1: return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
     *       default: return 1;
     *   }
     */
    (void)key_id;   /* 消除未使用参数警告 */
    return 1;       /* 默认返回高电平 (未按下) */
}

/**
 * @brief  获取按键当前有效电平 (结合 KEY_PRESS_LEVEL 宏判断)
 * @param  key_id 按键索引
 * @retval 1: 按键处于按下状态, 0: 按键处于释放状态
 */
static uint8_t Key_IsPressed(uint8_t key_id)
{
    uint8_t pin_level = Key_ReadPin(key_id);
    return (pin_level == KEY_PRESS_LEVEL) ? 1 : 0;
}

/**
 * @brief  按键扫描函数 (在 1ms 定时器中断中调用)
 * @note   使用状态机实现消抖、短按、长按、双击检测
 *
 * 状态机流程:
 *
 *   IDLE ──(按下)──> DEBOUNCE_PRESS ──(消抖通过)──> PRESSED ──(长按超时)──> [长按事件]
 *   ↑                                                      │
 *   │                                                   (释放)
 *   │                                                      ↓
 *   │                                            DEBOUNCE_RELEASE
 *   │                                                      │
 *   └── (超时, 短按) ── WAIT_DOUBLE_CLICK <── (短按确认释放)
 *                            │       ↑
 *                   (超时)   │       │ (第二次按下, second_click=1)
 *                     ↓      │       │
 *               [短按事件]   │       ↓
 *                            └── DEBOUNCE_PRESS ──> PRESSED ──(释放)──>
 *                            DEBOUNCE_RELEASE(second_click=1) ──> [双击事件] → IDLE
 */
void Key_Scan(void)
{
    uint8_t i;

    for (i = 0; i < KEY_COUNT; i++) {
        Key_t *k = &keys[i];

        switch (k->state) {

        /* -------------------- 空闲状态 -------------------- */
        case KEY_STATE_IDLE:
            if (Key_IsPressed(i)) {
                /* 检测到按下, 进入按下消抖 */
                k->debounce_count  = 0;
                k->timer           = 0;
                k->second_click    = 0;
                k->long_press_sent = 0;
                k->state = KEY_STATE_DEBOUNCE_PRESS;
            }
            break;

        /* -------------------- 按下消抖 --------------------
         *  连续采样到 KEY_DEBOUNCE_MS 次按下电平才算确认
         *  读到非按下电平只重置连续计数, 继续等待
         *  超过 KEY_DEBOUNCE_TIMEOUT_MS 仍未确认则放弃
         * ------------------------------------------------- */
        case KEY_STATE_DEBOUNCE_PRESS:
            k->timer++;
            if (Key_IsPressed(i)) {
                k->debounce_count++;
                if (k->debounce_count >= KEY_DEBOUNCE_MS) {
                    /* 连续采样通过, 确认有效按下 */
                    k->state = KEY_STATE_PRESSED;
                    k->timer = 0;  /* 重置用于长按计时 */
                    break;        /* 状态已转换, 跳过超时检查 */
                }
            } else {
                /* 读到非按下电平, 重置连续计数, 不放弃, 继续等待 */
                k->debounce_count = 0;
            }
            /* 超时保护: 防止抖动频繁导致状态机卡死 */
            if (k->timer >= KEY_DEBOUNCE_TIMEOUT_MS) {
                if (k->second_click) {
                    /* 第二次点击消抖超时, 降级为第一次短按事件 */
                    k->event = KEY_EVENT_SHORT_PRESS;
                    k->second_click = 0;
                }
                k->state = KEY_STATE_IDLE;
            }
            break;

        /* -------------------- 已按下, 等待长按或释放 -------------------- */
        case KEY_STATE_PRESSED:
            if (Key_IsPressed(i)) {
                if (!k->second_click) {
                    /* 首次点击: 正常长按计时 */
                    k->timer++;
                    if (k->timer >= KEY_LONG_PRESS_MS && !k->long_press_sent) {
                        /* 长按触发 */
                        k->event           = KEY_EVENT_LONG_PRESS;
                        k->long_press_sent = 1;
                    }
                }
                /* second_click=1 时只等待释放确认双击, 不做长按计时 */
            } else {
                /* 检测到释放, 进入释放消抖 */
                k->debounce_count = 0;
                k->timer          = 0;
                k->state = KEY_STATE_DEBOUNCE_RELEASE;
            }
            break;

        /* -------------------- 释放消抖 --------------------
         *  连续采样到 KEY_DEBOUNCE_MS 次释放电平才算确认
         *  读到按下电平只重置连续计数, 继续等待
         *  超过 KEY_DEBOUNCE_TIMEOUT_MS 仍未确认则放弃
         * ------------------------------------------------- */
        case KEY_STATE_DEBOUNCE_RELEASE:
            k->timer++;
            if (!Key_IsPressed(i)) {
                k->debounce_count++;
                if (k->debounce_count >= KEY_DEBOUNCE_MS) {
                    /* 连续采样通过, 确认释放 */
                    if (k->second_click) {
                        /* 第二次点击释放确认 -> 双击事件 */
                        k->event        = KEY_EVENT_DOUBLE_CLICK;
                        k->second_click = 0;
                        k->state = KEY_STATE_IDLE;
                    } else if (k->long_press_sent) {
                        /* 长按后的释放, 直接回空闲 */
                        k->state = KEY_STATE_IDLE;
                    } else {
                        /* 短按释放, 进入双击等待窗口 */
                        k->state = KEY_STATE_WAIT_DOUBLE_CLICK;
                        k->timer = 0;
                    }
                    break;  /* 状态已转换, 跳过超时检查 */
                }
            } else {
                /* 读到按下电平, 重置连续计数, 不放弃, 继续等待 */
                k->debounce_count = 0;
            }
            /* 超时保护: 防止抖动频繁导致状态机卡死 */
            if (k->timer >= KEY_DEBOUNCE_TIMEOUT_MS) {
                if (k->second_click) {
                    /* 第二次点击释放超时, 两次按下已确认, 降级为双击 */
                    k->event = KEY_EVENT_DOUBLE_CLICK;
                    k->second_click = 0;
                } else if (!k->long_press_sent) {
                    /* 短按释放超时, 降级为短按事件, 避免丢失用户操作 */
                    k->event = KEY_EVENT_SHORT_PRESS;
                }
                /* long_press_sent=1 时长按事件已在 PRESSED 状态产生, 无需额外处理 */
                k->state = KEY_STATE_IDLE;
            }
            break;

        /* -------------------- 等待双击 -------------------- */
        case KEY_STATE_WAIT_DOUBLE_CLICK:
            if (Key_IsPressed(i)) {
                /* 在双击窗口内再次按下, 标记为第二次点击 */
                k->debounce_count = 0;
                k->timer          = 0;
                k->second_click   = 1;
                k->state = KEY_STATE_DEBOUNCE_PRESS;
            } else {
                k->timer++;
                if (k->timer >= KEY_DOUBLE_CLICK_MS) {
                    /* 双击超时 -> 短按事件 */
                    k->event = KEY_EVENT_SHORT_PRESS;
                    k->state = KEY_STATE_IDLE;
                }
            }
            break;

        default:
            k->state = KEY_STATE_IDLE;
            break;
        }
    }
}

/**
 * @brief  获取指定按键的事件 (在主循环中调用)
 * @param  key_id 按键索引, 范围 [0, KEY_COUNT)
 * @retval KeyEvent_t 按键事件, 读取后自动清除为 KEY_EVENT_NONE
 *
 * @note   使用示例:
 *   KeyEvent_t evt = Key_GetEvent(0);
 *   switch (evt) {
 *       case KEY_EVENT_SHORT_PRESS:  // 处理短按
 *           break;
 *       case KEY_EVENT_LONG_PRESS:   // 处理长按
 *           break;
 *       case KEY_EVENT_DOUBLE_CLICK: // 处理双击
 *           break;
 *       default:
 *           break;
 *   }
 */
KeyEvent_t Key_GetEvent(uint8_t key_id)
{
    if (key_id >= KEY_COUNT) {
        return KEY_EVENT_NONE;
    }

    KeyEvent_t evt = keys[key_id].event;

    /* 读取后清除事件 */
    if (evt != KEY_EVENT_NONE) {
        keys[key_id].event = KEY_EVENT_NONE;
    }

    return evt;
}

#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "co_printf.h"


#define SYS_LOG_ENABLE 1   // 1=启用日志, 0=禁用日志

// 日志标签定义
// const char *log_tag = "project";
//#define BLE_BUSS_INFO(format, ...) LOG_INFO(log_tag, format, ##__VA_ARGS__)
//#define BLE_BUSS_INFO_L(format, ...) LOG_INFO_L(format, ##__VA_ARGS__)

// // 十六进制数据转储
// LOG_HEX_INFO("RX_DATA", rx_buffer, rx_length);




//`co_printf()` 输出到UART
//需要将 `logger.c` 中所有的 `co_printf(...)` 替换为对应平台的串口输出函数
//#include "co_printf.h"

// 日志级别定义
typedef enum {
    LOG_LEVEL_NONE = 0, // 无日志
    LOG_LEVEL_ERROR,    // 错误日志
    LOG_LEVEL_WARN,     // 警告日志
    LOG_LEVEL_INFO,     // 信息日志
    LOG_LEVEL_DEBUG,    // 调试日志
    LOG_LEVEL_VERBOSE   // 详细日志
} log_level_t;

// 日志输出方式定义
typedef enum {
    LOG_OUTPUT_NONE = 0x00, // 无输出
    LOG_OUTPUT_UART = 0x01, // 串口输出
} log_output_t;

// 初始化日志系统
void logger_init(log_level_t level, log_output_t output);

// 设置日志级别
void logger_set_level(log_level_t level);

// 设置日志输出方式
void logger_set_output(log_output_t output);



// 基础日志函数
void logger_log(log_level_t level, const char *tag, const char *format, ...);

// 简易日志宏
#if SYS_LOG_ENABLE
#define LOG_ERROR(tag, format, ...) logger_log(LOG_LEVEL_ERROR, tag, format, ##__VA_ARGS__)
#define LOG_WARN(tag, format, ...) logger_log(LOG_LEVEL_WARN, tag, format, ##__VA_ARGS__)
#define LOG_INFO(tag, format, ...) logger_log(LOG_LEVEL_INFO, tag, format, ##__VA_ARGS__)
#define LOG_DEBUG(tag, format, ...) logger_log(LOG_LEVEL_DEBUG, tag, format, ##__VA_ARGS__)
#define LOG_VERBOSE(tag, format, ...) logger_log(LOG_LEVEL_VERBOSE, tag, format, ##__VA_ARGS__)
#else
#define LOG_ERROR(tag, format, ...)
#define LOG_WARN(tag, format, ...)
#define LOG_INFO(tag, format, ...)
#define LOG_DEBUG(tag, format, ...)
#define LOG_VERBOSE(tag, format, ...)
#endif


// 带文件行号的日志函数
void logger_log_with_line(log_level_t level, const char *file, int line, const char *format, ...);

// 带文件行号的日志宏
#if SYS_LOG_ENABLE
#define LOG_ERROR_L(format, ...) logger_log_with_line(LOG_LEVEL_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_WARN_L(format, ...) logger_log_with_line(LOG_LEVEL_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_INFO_L(format, ...) logger_log_with_line(LOG_LEVEL_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_DEBUG_L(format, ...) logger_log_with_line(LOG_LEVEL_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_VERBOSE_L(format, ...) logger_log_with_line(LOG_LEVEL_VERBOSE, __FILE__, __LINE__, format, ##__VA_ARGS__)
#else
#define LOG_ERROR_L(format, ...)
#define LOG_WARN_L(format, ...)
#define LOG_INFO_L(format, ...)
#define LOG_DEBUG_L(format, ...)
#define LOG_VERBOSE_L(format, ...)
#endif

// 十六进制数据输出函数
void logger_hex_dump(log_level_t level, const char *tag, const void *data, uint16_t length);

// 简易十六进制数据输出宏
#if SYS_LOG_ENABLE
#define LOG_HEX_ERROR(tag, data, length) logger_hex_dump(LOG_LEVEL_ERROR, tag, data, length)
#define LOG_HEX_WARN(tag, data, length) logger_hex_dump(LOG_LEVEL_WARN, tag, data, length)
#define LOG_HEX_INFO(tag, data, length) logger_hex_dump(LOG_LEVEL_INFO, tag, data, length)
#define LOG_HEX_DEBUG(tag, data, length) logger_hex_dump(LOG_LEVEL_DEBUG, tag, data, length)
#else
#define LOG_HEX_ERROR(tag, data, length)
#define LOG_HEX_WARN(tag, data, length)
#define LOG_HEX_INFO(tag, data, length)
#define LOG_HEX_DEBUG(tag, data, length)
#endif

#endif // LOGGER_H
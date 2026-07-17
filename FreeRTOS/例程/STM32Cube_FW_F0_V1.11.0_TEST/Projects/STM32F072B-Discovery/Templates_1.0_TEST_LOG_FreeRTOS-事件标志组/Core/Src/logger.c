#include "logger.h"

// 日志模块内部状态
typedef struct {
    log_level_t current_level;
    log_output_t output_mode;
    char log_buffer[256]; // 日志缓冲区
} logger_t;

static logger_t logger;

// 日志级别字符串
static const char *level_strings[] = {
    "NONE", "ERROR", "WARN", "INFO", "DEBUG", "VERBOSE"};

// // 日志级别颜色代码 (适用于支持ANSI颜色的终端)
// static const char *level_colors[] = {
//     "", "\033[1;31m", "\033[1;33m", "\033[1;32m", "\033[1;34m", "\033[1;36m"};

// 初始化日志系统
void logger_init(log_level_t level, log_output_t output) {
#if SYS_LOG_ENABLE
    logger.current_level = level;
    logger.output_mode = output;

    LOG_INFO("Logger", "Logger initialized, level: %s, output: 0x%02X",
             level_strings[level], output);
#endif
}

// 设置日志级别
void logger_set_level(log_level_t level) {
    logger.current_level = level;
    LOG_INFO("Logger", "Log level set to: %s", level_strings[level]);
}

// 设置日志输出方式
void logger_set_output(log_output_t output) {
    logger.output_mode = output;
    LOG_INFO("Logger", "Log output set to: 0x%02X", output);
}

/**
 * @brief  格式化日志消息，将级别、标签和可变参数拼接为完整字符串
 * @param  buffer       输出缓冲区，存放格式化后的日志字符串
 * @param  buffer_size  缓冲区大小（字节）
 * @param  level        日志级别，用于在消息前缀中显示级别名称
 * @param  tag          模块标签，用于标识日志来源
 * @param  format       用户格式化字符串（printf 风格）
 * @param  args         已初始化的 va_list，对应 format 中的可变参数
 * @retval >0           成功写入的字节数（含 \r\n，不含末尾 \0）
 * @retval -1           格式化失败或缓冲区溢出
 * @note   输出格式为 "[LEVEL] tag: message\r\n"
 *         缓冲区不足时截断内容，但保证以 \r\n\0 结尾
 */
static int format_log_message(char *buffer, int buffer_size, log_level_t level,
                              const char *tag, const char *format, va_list args) {
    // 获取当前时间戳
    // uint32_t timestamp = get_timestamp_ms();

    // 格式化日志前缀 最多写入 buffer_size - 1 个字符  自动添加\0
    int prefix_len = snprintf(buffer, buffer_size,
                              "[%s] %s: ", level_strings[level], tag);

    if (prefix_len < 0 || prefix_len >= buffer_size) {
        return -1; // 缓冲区溢出
    }

    // 处理可变参数  最多写入 buffer_size - prefix_len - 1 个字符  自动添加\0
    int message_len = vsnprintf(buffer + prefix_len, buffer_size - prefix_len, format, args);

    if (message_len < 0) {
        return -1; // 格式化错误
    }

    // 添加换行符
    int total_len = prefix_len + message_len;
    if (total_len < buffer_size - 2) {
        buffer[total_len] = '\r';
        buffer[total_len + 1] = '\n';
        buffer[total_len + 2] = '\0';
        return total_len + 2;
    } else {
        // 缓冲区已满，确保以换行符结束
        buffer[buffer_size - 3] = '\r';
        buffer[buffer_size - 2] = '\n';
        buffer[buffer_size - 1] = '\0';
        return buffer_size - 1;
    }
}

/**
 * @brief  基础日志输出函数，按当前级别和输出模式输出日志
 * @param  level   日志级别，低于当前设定级别或为 NONE 时直接返回
 * @param  tag     模块标签，标识日志来源
 * @param  format  格式化字符串（printf 风格）
 * @param  ...     可变参数，对应 format 中的占位符
 * @note   仅在 output_mode 包含 LOG_OUTPUT_UART 时输出到串口
 */
void logger_log(log_level_t level, const char *tag, const char *format, ...) {
    // 检查日志级别
    if (level > logger.current_level || level == LOG_LEVEL_NONE) {
        return;
    }

    // 格式化日志消息
    va_list args;  // 定义可变参数列表指针
    va_start(args, format); // 初始化 args，从 format 参数之后开始定位可变参数
    int length = format_log_message(logger.log_buffer, sizeof(logger.log_buffer),
                                    level, tag, format, args);
    va_end(args);  // 清理 args，结束可变参数的访问

    if (length <= 0) {
        return; // 格式化失败
    }

    // 输出到串口
    if (logger.output_mode & LOG_OUTPUT_UART) {
        CDC_Transmit_FS((uint8_t*)logger.log_buffer,strlen(logger.log_buffer));
    }
}

/**
 * @brief  带文件名和行号的日志输出函数，自动提取源文件名并拼接到标签中
 * @param  level   日志级别，低于当前设定级别或为 NONE 时直接返回
 * @param  file    源文件路径（通常传入 __FILE__），自动提取裸文件名
 * @param  line    源文件行号（通常传入 __LINE__）
 * @param  format  格式化字符串（printf 风格）
 * @param  ...     可变参数，对应 format 中的占位符
 * @note   输出格式为 "[LEVEL] filename:line: message\r\n"
 *         同时支持 '/' 和 '\\' 路径分隔符，兼容不同平台
 */
void logger_log_with_line(log_level_t level, const char *file, int line, const char *format, ...) {
    // 检查日志级别
    if (level > logger.current_level || level == LOG_LEVEL_NONE) {
        return;
    }

    // 提取文件名 (去掉路径)
    const char *filename = file;
    const char *slash = strrchr(file, '/');  // 查找最后一个 '/' 或 '\\'
    if (slash == NULL) {
        slash = strrchr(file, '\\');
    }
    if (slash != NULL) {
        filename = slash + 1;
    }

    // 格式化带行号的标签
    char tag_with_line[64];
    snprintf(tag_with_line, sizeof(tag_with_line), "%s:%d", filename, line);

    // 使用基础日志函数
    va_list args;
    va_start(args, format);
    int length = format_log_message(logger.log_buffer, sizeof(logger.log_buffer),
                                    level, tag_with_line, format, args);
    va_end(args);

    if (length <= 0) {
        return; // 格式化失败
    }

    // 输出到串口
    if (logger.output_mode & LOG_OUTPUT_UART) {
        CDC_Transmit_FS((uint8_t*)logger.log_buffer,strlen(logger.log_buffer));
    }
}

/**
 * @brief  十六进制数据转储函数，将二进制数据以 hex+ASCII 格式分块输出
 * @param  level   日志级别，低于当前设定级别或为 NONE 时直接返回
 * @param  tag     模块标签，标识日志来源
 * @param  data    待转储的二进制数据指针
 * @param  length  数据长度（字节）
 * @note   每行输出16字节，格式为 "[LEVEL] tag: 0000: XX XX ... XX | ASCII\r\n"
 *         不可打印字符（0x20~0x7E 范围外）显示为 '.'
 *         缓冲区不足时截断当前行，但保证以 \r\n\0 结尾
 */
void logger_hex_dump(log_level_t level, const char *tag, const void *data, uint16_t length) {
    // 检查日志级别
    if (level > logger.current_level || level == LOG_LEVEL_NONE) {
        return;
    }

    // 检查输出模式
    if (logger.output_mode == LOG_OUTPUT_NONE) {
        return;
    }

    const uint8_t *bytes = (const uint8_t *)data;
    uint16_t offset = 0;

    // 分块处理数据，每块16字节
    while (offset < length) {
        uint16_t chunk_size = (length - offset) > 16 ? 16 : (length - offset);

        // 获取当前时间戳
        // uint32_t timestamp = get_timestamp_ms();

        // 格式化日志前缀
        int prefix_len = snprintf(logger.log_buffer, sizeof(logger.log_buffer),
                                  "[%s] %s: %04X: ",
                                  level_strings[level], tag, offset);

        if (prefix_len < 0 || prefix_len >= (int)sizeof(logger.log_buffer)) {
            return; // 缓冲区溢出
        }

        // 计算剩余缓冲区大小
        int remaining = sizeof(logger.log_buffer) - prefix_len;
        int current_pos = prefix_len;

        // 转换数据为十六进制字符串
        for (int i = 0; i < chunk_size && remaining > 3; i++) {
            int bytes_written = snprintf(logger.log_buffer + current_pos, remaining, "%02X ", bytes[offset + i]);
            if (bytes_written < 0) {
                break;
            }
            current_pos += bytes_written;
            remaining -= bytes_written;
        }

        // 添加ASCII显示部分
        if (remaining > chunk_size + 2) {
            // 添加分隔符
            int sep_len = snprintf(logger.log_buffer + current_pos, remaining, "| ");
            if (sep_len > 0) {
                current_pos += sep_len;
                remaining -= sep_len;
            }

            // 添加ASCII字符
            for (int i = 0; i < chunk_size && remaining > 1; i++) {
                uint8_t byte = bytes[offset + i];
                char display_char = (byte >= 0x20 && byte <= 0x7E) ? byte : '.';
                logger.log_buffer[current_pos++] = display_char;
                remaining--;
            }
        }

        // 添加换行符
        if (current_pos < (int)sizeof(logger.log_buffer) - 2) {
            logger.log_buffer[current_pos] = '\r';
            logger.log_buffer[current_pos + 1] = '\n';
            logger.log_buffer[current_pos + 2] = '\0';
            current_pos += 2;
        } else {
            // 缓冲区已满，确保以换行符结束
            logger.log_buffer[sizeof(logger.log_buffer) - 3] = '\r';
            logger.log_buffer[sizeof(logger.log_buffer) - 2] = '\n';
            logger.log_buffer[sizeof(logger.log_buffer) - 1] = '\0';
            current_pos = sizeof(logger.log_buffer) - 1;
        }

        // 输出到串口
        if (logger.output_mode & LOG_OUTPUT_UART) {
        CDC_Transmit_FS((uint8_t*)logger.log_buffer,strlen(logger.log_buffer));
        }

        offset += chunk_size;
    }
}
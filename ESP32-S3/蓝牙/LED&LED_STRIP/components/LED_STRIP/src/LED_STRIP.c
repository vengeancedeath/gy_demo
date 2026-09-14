
#include "LED_STRIP.h"
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "esp_timer.h"


#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define RMT_LED_STRIP_GPIO_NUM      48          // 输出 GPIO = 8

#define EXAMPLE_LED_NUMBERS         1          // 灯珠数量

#define EXAMPLE_FRAME_DURATION_MS   1000          // 帧持续时间，越小越快，单位毫秒

static const char *TAG = "LED_STRIP";

static uint8_t led_strip_pixels[EXAMPLE_LED_NUMBERS * 3];

//RMT 的分辨率设为 10MHz，所以 1 tick = 0.1μs，3 ticks = 0.3μs，9 ticks = 0.9μs。
//bit 0
static const rmt_symbol_word_t ws2812_zero = { 
    .level0 = 1, //第一段电平的值（1=高，0=低）
    .duration0 = 0.3 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T0H=0.3us 第一段电平的持续时间 tick数
    .level1 = 0, //第二段电平的值（1=高，0=低）
    .duration1 = 0.9 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T0L=0.9us 第二段电平的持续时间 tick数
};

//bit 1
static const rmt_symbol_word_t ws2812_one = {
    .level0 = 1,
    .duration0 = 0.9 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T1H=0.9us
    .level1 = 0,
    .duration1 = 0.3 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T1L=0.3us
};

//reset defaults to 50uS
static const rmt_symbol_word_t ws2812_reset = {
    .level0 = 0,
    .duration0 = RMT_LED_STRIP_RESOLUTION_HZ / 1000000 * 50 / 2,
    .level1 = 0,
    .duration1 = RMT_LED_STRIP_RESOLUTION_HZ / 1000000 * 50 / 2,
};

static size_t encoder_callback(
    const void *data,                   // 你传入的原始数据（RGB 像素数组）
    size_t data_size,                   // 原始数据的总字节数
    size_t symbols_written,             // 之前已经写入了多少个 RMT 符号
    size_t symbols_free,                // 当前缓冲区还能放多少个符号
    rmt_symbol_word_t *symbols,         // 你要往这里填符号
    bool *done,                         // 设为 true 表示编码结束
    void *arg)                          // 用户自定义参数（本例未用）
{
    // We need a minimum of 8 symbol spaces to encode a byte. We only
    // need one to encode a reset, but it's simpler to simply demand that
    // there are 8 symbol spaces free to write anything.
    if (symbols_free < 8) {
        return 0;
    }

    // We can calculate where in the data we are from the symbol pos.
    // Alternatively, we could use some counter referenced by the arg
    // parameter to keep track of this.
    //从最高位（MSB）开始，因为 WS2812 要求高位先发
    size_t data_pos = symbols_written / 8;
    uint8_t *data_bytes = (uint8_t*)data;
    if (data_pos < data_size) {
        // Encode a byte
        size_t symbol_pos = 0;
        for (int bitmask = 0x80; bitmask != 0; bitmask >>= 1) {
            if (data_bytes[data_pos]&bitmask) {
                symbols[symbol_pos++] = ws2812_one;  // 1 位
            } else {
                symbols[symbol_pos++] = ws2812_zero; // 0 位
            }
        }
        // We're done; we should have written 8 symbols.
        return symbol_pos;
    } else {
        //All bytes already are encoded.
        //Encode the reset, and we're done.
        symbols[0] = ws2812_reset;  // 重置信号
        *done = 1; //Indicate end of the transaction.
        return 1; //we only wrote one symbol
    }
}

rmt_transmit_config_t tx_config = {0};
rmt_channel_handle_t led_chan = NULL;
rmt_encoder_handle_t simple_encoder = NULL;

void led_strip_init(void)
{
    ESP_LOGI(TAG, "Create RMT TX channel");
    //1. 创建 RMT 通道句柄
    // rmt_channel_handle_t led_chan = NULL; 放全局了
    //2. 配置 RMT 通道
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // 使用默认时钟源
        .gpio_num = RMT_LED_STRIP_GPIO_NUM,    // 输出 GPIO = 8
        .mem_block_symbols = 64, //  RMT 内存块大小，64个符号
        // RMT 硬件一次最多能装 64 个符号。如果灯珠很多（比如 24 颗 × 3 字节 × 8 bit = 576 个符号），需要分多次发送。
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ, // 10MHz 分辨率
        .trans_queue_depth = 4, //  最多4个传输可以排队
    };
    //3. 创建 RMT 通道
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));

    ESP_LOGI(TAG, "Create simple callback-based encoder");
    //4. 创建 RMT 编码器句柄
    // rmt_encoder_handle_t simple_encoder = NULL; 放全局了
    //5. 配置 RMT 编码器
    const rmt_simple_encoder_config_t simple_encoder_cfg = {
        .callback = encoder_callback   // 只需指定回调函数
        //Note we don't set min_chunk_size here as the default of 64 is good enough.
    };
    //6. 创建 RMT 编码器
    ESP_ERROR_CHECK(rmt_new_simple_encoder(&simple_encoder_cfg, &simple_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    //7. 启用 RMT 通道
    ESP_ERROR_CHECK(rmt_enable(led_chan));

    ESP_LOGI(TAG, "Start LED rainbow chase");

    //8. 配置 RMT 传输参数
    // rmt_transmit_config_t tx_config = {
    //     .loop_count = 0, // no transfer loop 无循环传输
    // };
    tx_config.loop_count = 0; // no transfer loop 无循环传输
}

void led_strip_show(uint8_t color_r, uint8_t color_g, uint8_t color_b)
{
    for (int led = 0; led < EXAMPLE_LED_NUMBERS; led++) {
          
            led_strip_pixels[led * 3 + 0] = color_r;//G 
            led_strip_pixels[led * 3 + 1] = color_g;// R
            led_strip_pixels[led * 3 + 2] = color_b;// B
        }
        // Flush RGB values to LEDs
        //9. 发送 RMT 信号
        ESP_ERROR_CHECK(rmt_transmit(led_chan, simple_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
        //10. 等待 RMT 传输完成  OR rmt_tx_register_event_callbacks 注册事件完成回调
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(led_chan, portMAX_DELAY));
        //11. 延时 20ms，等待下一个帧
        // vTaskDelay(pdMS_TO_TICKS(EXAMPLE_FRAME_DURATION_MS));
        //Increase offset to shift pattern

        ESP_LOGI(TAG, "[%lld ms] RGB_R: %d, RGB_G: %d, RGB_B: %d", esp_timer_get_time() / 1000, color_r, color_g, color_b);
     
}

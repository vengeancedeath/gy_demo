#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_TEST    0
#if LED_TEST
#include "LED.h"
#endif



#define LED_STRIP_TEST    1
#if LED_STRIP_TEST
#include "LED_STRIP.h"
#endif



void app_main(void)
{
#if LED_TEST
    LED_init();
    printf("Hello world!\n");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    while(1)
    {
        LED_toggle();
        LED_log();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
#endif

#if LED_STRIP_TEST
    led_strip_init();
    while(1)
    {
        led_strip_show(255, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        led_strip_show(0, 255, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        led_strip_show(0, 0, 255);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
#endif
}

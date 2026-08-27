#include <stdio.h>
#include "LED.h"
#include "driver/gpio.h"
#include "esp_err.h"

#define GPIO_OUTPUT_PIN        GPIO_NUM_10
#define GPIO_OUTPUT_PIN_SEL  (1ULL<< GPIO_OUTPUT_PIN)

void LED_init(void)
{

     gpio_config_t io_conf = {0};
     esp_err_t ret = ESP_OK;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    ret =  gpio_config(&io_conf);
    if(ret != ESP_OK)
    {
        printf("LED_init: gpio_config failed, ret = %d\n", ret);
    }
}

void LED_toggle(void)
{
    gpio_set_level(GPIO_OUTPUT_PIN, !gpio_get_level(GPIO_OUTPUT_PIN));
}

void LED_log(void)
{
    printf("LED_log: %d\n", gpio_get_level(GPIO_OUTPUT_PIN));
    
}
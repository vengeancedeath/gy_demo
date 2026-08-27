#ifndef __LED_STRIP_H__
#define __LED_STRIP_H__
#include <stdio.h>
#include <stdint.h>
void led_strip_init(void);
void led_strip_show(uint8_t color_r, uint8_t color_g, uint8_t color_b);

#endif

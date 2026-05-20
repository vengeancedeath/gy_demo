/*
 * Common.C
 *
 *  Created on: Nov 15, 2024
 *      Author: junjie.lai
 */

#include "common.h"
#include "main.h"
#include "stdio.h"


// 初始化电源
void power_init()
{
	HAL_GPIO_WritePin(EN_3V3_GPIO_Port, EN_3V3_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(EN_1V8_GPIO_Port, EN_1V8_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	// HAL_GPIO_WritePin(EN_1V2_GPIO_Port, EN_1V2_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	// HAL_GPIO_WritePin(EN_VPP_GPIO_Port, EN_VPP_Pin, GPIO_PIN_SET); // volgate 7.5V not turn on
	HAL_Delay(1);
}
//没有调用
void power_off()
{
	HAL_GPIO_WritePin(EN_3V3_GPIO_Port, EN_3V3_Pin, 0);
	HAL_Delay(1);
	HAL_GPIO_WritePin(EN_1V8_GPIO_Port, EN_1V8_Pin, 0);
	HAL_Delay(1);
	// HAL_GPIO_WritePin(EN_1V2_GPIO_Port, EN_1V2_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	// HAL_GPIO_WritePin(EN_VPP_GPIO_Port, EN_VPP_Pin, GPIO_PIN_SET); // volgate 7.5V not turn on
	HAL_Delay(1);
}
//没有调用
void all_oled_reset()
{
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET);

	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET);

	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET);

	HAL_Delay(200);
}


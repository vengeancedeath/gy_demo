/*
 * qspi.c
 *
 *  Created on: Nov 15, 2024
 *      Author: junjie.lai
 */
#include "qspi.h"
#include "main.h"
#include "stdio.h"
#include "quadspi.h"
#include "gpio.h"
#include "common.h"

uint8_t cd80[5] = {0x02, 0x00, 0x80, 0x00, 0X01};
uint8_t cd90[7] = {0x02, 0x00, 0x90, 0x00, 0X74, 0x3D, 0x0A}; // 0X70, 0x14
uint8_t cd91[5] = {0x02, 0x00, 0x91, 0x00, 0X90};
uint8_t cd89[8] = {0x02, 0x00, 0x89, 0x00, 0XF1, 0xF1, 0X00, 0x09};
uint8_t cd8A[8] = {0x02, 0x00, 0x8A, 0x00, 0x18, 0x03, 0x07, 0x04};
uint8_t cd96[6] = {0x02, 0x00, 0x96, 0x00, 0x31, 0x00};
uint8_t cd97[6] = {0x02, 0x00, 0x97, 0x00, 0X01, 0x00};
uint8_t cd95[6] = {0x02, 0x00, 0x95, 0x00, 0x81, 0x7F}; // 0x81, 0x7F, 0x00

uint8_t cd51[6] = {0x02, 0x00, 0x51, 0x00, 0XFF, 0x03};

uint8_t cd36[5] = {0x02, 0x00, 0x36, 0x00, 0X02};

uint8_t cd35[5] = {0x02, 0x00, 0x35, 0x00, 0X00};
uint8_t cd44[5] = {0x02, 0x00, 0x44, 0x00, 0X01};
uint8_t cdA8[5] = {0x02, 0x00, 0xA8, 0x00, 0X01};
uint8_t cdAA[5] = {0x02, 0x00, 0xAA, 0x00, 0X14}; // 08  //0x01
uint8_t cdAA2[5] = {0x02, 0x00, 0xAA, 0x00, 0X01};
uint8_t cdA9[5] = {0x02, 0x00, 0xA9, 0x00, 0X01};
uint8_t cd11[5] = {0x02, 0x00, 0x11, 0x00, 0X00};
uint8_t cd10[5] = {0x02, 0x00, 0x10, 0x00, 0X00};
uint8_t cd29[5] = {0x02, 0x00, 0x29, 0x00, 0X00};
uint8_t cd28[5] = {0x02, 0x00, 0x28, 0x00, 0X00};
uint8_t cdF0[6] = {0x02, 0x00, 0xF0, 0x00, 0x0D << 3 | 0x01, 0x04};
uint8_t cdF1[7] = {0x02, 0x00, 0xF1, 0x00, 0xFF, 0xFF, 0xFF};





//OLED复位
void oled_QSPI_reset()
{
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
}


/*-----------------------QSPI_点图-----------------------------  */


void oled_QSPI_init()
{
	uint8_t date[6] = {0x05};
	power_init(); // 初始化电源
	oled_QSPI_reset(); // 重置QSPI

	//配置寄存器0x80
	qspi_instruct(cd80, 5);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	// qspi_tx_done = 0;
	// oled_QSPI_read_register(CmdA1, date, 1);
	// HAL_Delay(10);

	//配置寄存器0x90
	qspi_instruct(cd90, 7);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	qspi_instruct(cd91, 5);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	qspi_instruct(cd89, 8);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	// qspi_instruct(cd36, 8); // 用于镜像
	// while (qspi_tx_done == 0)
	// 	;
	// qspi_tx_done = 0;
	// HAL_Delay(10);

	qspi_instruct(cd8A, 8);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	qspi_instruct(cd96, 6);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	qspi_instruct(cd97, 6);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	qspi_instruct(cd95, 6);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	qspi_instruct(cd51, 6);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	qspi_instruct(cd35, 5);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	qspi_instruct(cd44, 5);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(10);

	// qspi_instruct(cd35, 5);
	// while (qspi_tx_done == 0)
	// 	;
	// qspi_tx_done = 0;
	// oled_QSPI_read_register(Cmd35, date, 1);
	// HAL_Delay(10);
	/* ------------------------------亮度调节-------------------------------- */
	{
		qspi_instruct(cdA8, 5);
		while (qspi_tx_done == 0)
			;
		qspi_tx_done = 0;
		HAL_Delay(10);

		qspi_instruct(cdAA, 5);
		while (qspi_tx_done == 0)
			;
		qspi_tx_done = 0;
		HAL_Delay(10);

		qspi_instruct(cdA9, 5);
		while (qspi_tx_done == 0)
			;
		qspi_tx_done = 0;
		HAL_Delay(10);
	}
	// 设置elvdd为高，拉高电平
	HAL_GPIO_WritePin(EN_ELVDD_GPIO_Port, EN_ELVDD_Pin, GPIO_PIN_SET);
	HAL_Delay(100);

	qspi_instruct(cd11, 5);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	HAL_Delay(100);

	//	HAL_GPIO_WritePin(EN_ELVDD_GPIO_Port, EN_ELVDD_Pin, GPIO_PIN_SET);
	//	HAL_Delay(100);

	//等待TE信号（垂直同步）
	while (te_cmd == 0)
		;
	qspi_instruct(cd29, 5);
	while (qspi_tx_done == 0)
		;
	qspi_tx_done = 0;
	// HAL_Delay(100);
	//  设置elvss为-6v，拉高电平
	//	gImage_clear() ;

	// Dis_Play(); // 没有这个会爆亮
	// HAL_Delay(5);

	//	Dis_Play();
	//	HAL_Delay(10);
	//	Dis_Play();
	//	HAL_Delay(10);
	//	Dis_Play();
	// 使能ELVSS（负电压）
	HAL_GPIO_WritePin(EN_ELVSS_GPIO_Port, EN_ELVSS_Pin, GPIO_PIN_SET);
	//	Dis_Play();
	//	Dis_Play();
	//	Dis_Play();

	//	HAL_Delay(1000);
}



void QSPI_delay(uint16_t j)
{
	for (uint16_t i = 0; i < j; i++)
		;
}



void QSPI_send_image(const uint8_t image[][ROW])
{
	// 等待TE信号（垂直同步）
	teflag = 0;
	while (teflag == 0)
	{
	}
	teflag = 0;
	// 发送第一行数据
	qspi_send_message((uint8_t *)image[0], sizeof(image[0]));
	while (qspi_tx_done == 0)
	{
	}
	qspi_tx_done = 0;
	QSPI_delay(70);
	// 发送其他行数据
	for (uint16_t column = 1; column < COLUMN; column++)
	{
		qspi_send_message((uint8_t *)image[column], sizeof(image[column]));
		while (qspi_tx_done == 0)
		{
		}
		qspi_tx_done = 0;
		QSPI_delay(70);
	}
}



/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include <stdbool.h>
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0; //自身从机地址设为 0。因为本设备工作在 __主机模式__，OwnAddress1 在主机模式下不使用，设为 0 即可。
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;  //使用标准的 7 位 I2C 地址
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;  //禁用双地址模式 主机模式不需要双地址模式
  hi2c1.Init.OwnAddress2 = 0;	 // 第二地址无用，设0
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;  // 第二地址无掩码
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;  // 禁用通用呼叫（广播）
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;  //__允许从机进行时钟拉伸__（Clock Stretching），即从机可以拉低 SCL 来暂停通信
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/*--------------------------------------------------------------------------------------
 * Function: Module_Write
 * Para: uint16_t RegAdd, uint8_t CMD
 * Return: None
 * Description:
 * 1. RDP702F I2C write command subroutine
 *
--------------------------------------------------------------------------------------*/

bool Module_Write(uint16_t RegAdd, uint8_t CMD){

	uint8_t Temp[3];

	Temp[2] = CMD;
	Temp[1] = RegAdd;
	Temp[0] = RegAdd >> 8;

	if(HAL_I2C_Master_Transmit(&hi2c1, Module_Write_Addr, Temp,3, 10000)== HAL_OK){
		USB_Printf("Reg 0x%04X write 0x%02X OK! \n", RegAdd, CMD);
		//HAL_Delay(5);
		return true;
	}
	else{
		USB_Printf("Reg 0x%04X write 0x%02X Fail! \n", RegAdd, CMD);
		USB_Printf("Please check the connection path between EVK and module. \r\n");
		return false;
	}
	HAL_Delay(100);
}


bool Module_Write_L011(uint16_t RegAdd, uint8_t CMD,uint8_t mode){

	uint8_t Temp[4];

	Temp[3] = mode;
	Temp[2] = CMD;
	Temp[1] = RegAdd;
	Temp[0] = RegAdd >> 8;

	if(HAL_I2C_Master_Transmit(&hi2c1, Module_Write_L011_Addr, Temp,4, 10000)== HAL_OK){
		USB_Printf("Reg 0x%04X write 0x%02X OK! \n", RegAdd, CMD);
		//HAL_Delay(5);
		return true;
	}
	else{
		USB_Printf("Reg 0x%04X write 0x%02X Fail! \n", RegAdd, CMD);
		USB_Printf("Please check the connection path between EVK and module. \r\n");
		return false;
	}
}

////bool EEROM_Write(uint16_t RegAdd, uint8_t CMD){
////Eric_H:026414
//bool EEROM_Write(uint8_t RegAdd, uint8_t CMD){
//
//	//uint8_t Temp[3];
//	//Eric_H:026410
//	uint8_t Temp[2];
//
//	//Temp[2] = CMD;
//	//Temp[1] = RegAdd;
//	//Temp[0] = RegAdd >> 8;
//	//Eric_H:026411
//	Temp[1] = CMD;
//	Temp[0] = RegAdd;
//
//	//if(HAL_I2C_Master_Transmit(&hi2c1, EEROM_Write_Addr, Temp,3, 10000)== HAL_OK){
//	//Eric_H:026411
//	if(HAL_I2C_Master_Transmit(&hi2c1, EEROM_Write_Addr, Temp,2, 358)== HAL_OK){
//		//USB_Printf("Reg 0x%04X write 0x%02X OK! \n", RegAdd, CMD);
//		//Eric_H:026411
//		USB_Printf("EEPROM Reg 0x%02X write 0x%02X OK! \n", RegAdd, CMD);
//		//HAL_Delay(5);
//		return true;
//	}
//	else{
//		//USB_Printf("Reg 0x%04X write 0x%02X Fail! \n", RegAdd, CMD);
//		//USB_Printf("Please check the connection path between EVK and module. \r\n");
//		//Eric_H:026411
//		USB_Printf("EEPROM Reg 0x%02X write 0x%02X Fail! \n", RegAdd, CMD);
//		USB_Printf("Please check the connection path between EVK and EEPROM. \r\n");
//		return false;
//	}
//}


//Eric_H:026411
bool I2Cdevice_Write(uint8_t I2Cdevice_Write_Addr, uint16_t RegAdd, uint8_t CMD){
	uint8_t Temp[3];

	Temp[2] = CMD;
	Temp[1] = RegAdd;
	Temp[0] = RegAdd >> 8;

	if(HAL_I2C_Master_Transmit(&hi2c1, I2Cdevice_Write_Addr, Temp,3, 10000)== HAL_OK){
		////USB_Printf("Reg 0x%04X write 0x%02X OK! for I2C device of 0x%02X address\n", RegAdd, CMD, I2Cdevice_Write_Addr);
		//HAL_Delay(5);
		return true;
	}
	else{
		USB_Printf("Reg 0x%04X write 0x%02X Fail! \n", RegAdd, CMD);
		USB_Printf("Please check the connection path between MPU and I2C device of 0x%02X address\r\n", I2Cdevice_Write_Addr);
		return false;
	}
}


//Eric_H:026414
bool I2Cdevice_Write8(uint8_t I2Cdevice_Write_Addr, uint8_t RegAdd, uint8_t CMD)
{
	uint8_t Temp[2];

	Temp[1] = CMD;
	Temp[0] = RegAdd;

	if(HAL_I2C_Master_Transmit(&hi2c1, I2Cdevice_Write_Addr, Temp, 2, 10000)== HAL_OK){
	////USB_Printf("Reg 0x%02X write 0x%02X OK! for I2C device of 0x%02X address\n", RegAdd, CMD, I2Cdevice_Write_Addr);
	//HAL_Delay(5);
	return true;
}
else{
	USB_Printf("Reg 0x%02X write 0x%02X Fail! \n", RegAdd, CMD);
	USB_Printf("Please check the connection path between MPU and I2C device of 0x%02X address\r\n", I2Cdevice_Write_Addr);
	return false;
}
}

//Eric_H:026414
bool I2Cdevice_Write8N(uint8_t I2Cdevice_Write_Addr, uint8_t RegAdd, uint8_t count, uint8_t *data)
{
	
	uint8_t i;
	//uint8_t Temp[count+1];
	uint8_t Temp[16+1];
	Temp[0] = RegAdd;
	//Temp[1] = CMD;
	for ( uint8_t i=0; i<count; i++) {
		Temp[i+1]=*(data+i);
	}

	if(HAL_I2C_Master_Transmit(&hi2c1, I2Cdevice_Write_Addr, Temp, count+1, 10000)== HAL_OK){
		USB_Printf("Reg 0x%02X write ", RegAdd);
		for (i=0; i<count; i++) {
			USB_Printf("0x%02X ", Temp[i+1]);
		}
		////USB_Printf("OK! for I2C device of 0x%02X address\n", I2Cdevice_Write_Addr);
		//HAL_Delay(5);
		return true;
	}
	else{
		USB_Printf("Reg 0x%02X write 0x%02X Fail! \n", RegAdd, Temp[1]);
		USB_Printf("Please check the connection path between MPU and I2C device of 0x%02X address\r\n", I2Cdevice_Write_Addr);
		return false;
	}
}

//Eric_H:026412
bool LT6911C_i2c_wr(uint8_t lt6911c_write_addr, uint16_t reg, uint8_t n, uint8_t *values)
{
	int i;
	//uint8_t data[n];
	uint8_t data[16];
	uint8_t buf[2] = { 0xFF, reg >> 8};

	//for (i = 0; i < 18; i++){
	//	USB_Printf("0x%02X ", values[i]);
	//	}
	
	data[0] = reg & 0xff;
	for (i = 0; i < n; i++){
		//data[1 + i] = values[i+7];
		//USB_Printf("0x%02X-%02X-0x%02X ", data[i], i, values[i+7]);
		data[1 + i] = values[i];
		////USB_Printf("0x%02X-%02X-0x%02X ", data[i], i, values[i]);
		}

	if(HAL_I2C_Master_Transmit(&hi2c1, lt6911c_write_addr, buf, 2, 10000)== HAL_OK){
		////USB_Printf("\n0x%02X-00 0x%02X-01\n", buf[0], buf[1]);
		//HAL_Delay(5);
	}
	else{
		USB_Printf("Bank REG 0x%02X%02X write Fail! for LT6911C of 0x%02X address \n", buf[0], buf[1], lt6911c_write_addr);
		return false;
	}

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready
	
	if(HAL_I2C_Master_Transmit(&hi2c1, lt6911c_write_addr, data, n+1, 10000)== HAL_OK){
		////USB_Printf("Addr and Data REG 0x%02X write OK! \n", data[0]);
		HAL_Delay(1);
		
		for(i = 0; i<=n; i++){
			USB_Printf("0x%02X-%02X ", data[i], i);
		}
		return true;
	}
	else{
		USB_Printf("Addr and Data REG 0x%02X write Fail! for LT6911C of 0x%02X address \n", data[0], lt6911c_write_addr);
	}
	return true;
}

/*--------------------------------------------------------------------------------------
 * Function: Module_Read
 * Para: uint16_t RegAdd, uint8_t CMD
 * Return: uint8_t Receive
 * Description:
 * 1. RDP702F I2C read command subroutine
 *
--------------------------------------------------------------------------------------*/
uint8_t Module_Read(uint16_t RegAdd){

	uint8_t Receive = 0; // read back register setting value and initial value is 0
	uint8_t Temp[2] ;

	Temp[1] =  RegAdd;
	Temp[0] =  RegAdd >> 8;


	if(HAL_I2C_Master_Transmit(&hi2c1, Module_Read_Addr, Temp , 2, 10000)== HAL_OK){
		//USB_Printf("Reg 0x%04X write OK! (In Read mode) \n", RegAdd);
		HAL_Delay(1);
	}
	else{
		USB_Printf("Reg 0x%04X write Fail! (In Read mode) \n", RegAdd);
	}

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready

	if(HAL_I2C_Master_Receive(&hi2c1, Module_Read_Addr , (uint8_t*)&Receive, 1, 10000) == HAL_OK){

		USB_Printf("Reg 0x%04X = 0x%02X. (In Read mode) \n", RegAdd, Receive);
		HAL_Delay(10);
	}
	else{
		USB_Printf("Reg 0x%04X read fail! (In Read mode) \n", RegAdd);

	}

	return (Receive); // return the value
}

uint8_t Module_Read_L011(uint16_t RegAdd){

	uint8_t Receive = 0; // read back register setting value and initial value is 0
	uint8_t Temp[4] ;

	Temp[3] =  0;
	Temp[2] =  0;
	Temp[1] =  RegAdd;
	Temp[0] =  RegAdd >> 8;


	if(HAL_I2C_Master_Transmit(&hi2c1, Module_Write_L011_Addr, Temp , 4, 14000)== HAL_OK){
		USB_Printf("Reg 0x%04X write OK! (In Read mode) \n", RegAdd);
		HAL_Delay(1);
	}
	else{
		USB_Printf("Reg 0x%04X write Fail! (In Read mode) \n", RegAdd);
	}

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready

	if(HAL_I2C_Master_Receive(&hi2c1, Module_Write_L011_Addr , (uint8_t*)&Receive, 1, 14000) == HAL_OK){

		USB_Printf("Reg 0x%04X = 0x%02X. (In Read mode) \n", RegAdd, Receive);
		HAL_Delay(10);
	}
	else{
		USB_Printf("Reg 0x%04X read fail! (In Read mode) \n", RegAdd);

	}

	return (Receive); // return the value
}


//uint8_t EEROM_Read(uint16_t RegAdd){
//
//	uint8_t Receive = 0; // read back register setting value and initial value is 0
//	//uint8_t Temp[2] ;
//	//Eric_H:026411
//	uint8_t Temp[1];
//
//	Temp[0] =  RegAdd;
//	//Eric_H:026411
//	//Temp[0] =  RegAdd >> 8;
//
//
//	//if(HAL_I2C_Master_Transmit(&hi2c1, EEROM_Read_Addr,Temp ,2, 10000)== HAL_OK){
//	//Eric_H:026411
//	if(HAL_I2C_Master_Transmit(&hi2c1, EEROM_Read_Addr,Temp ,1, 358)== HAL_OK){
//		//USB_Printf("Reg 0x%04X write OK! (In Read mode) \n", RegAdd);
//		//HAL_Delay(1);
//		//Eric_H:026410
//		HAL_Delay(8);
//	}
//	else{
//		//USB_Printf("Reg 0x%04X write Fail! (In Read mode) \n", RegAdd);
//		//Eric_H:026411
//		USB_Printf("EEPROM Reg 0x%02X write Fail! (In Read mode) \n", RegAdd);
//	}
//
//	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready
//
//	if(HAL_I2C_Master_Receive(&hi2c1, EEROM_Read_Addr , (uint8_t*)&Receive, 1, 10000) == HAL_OK){
//
//		//USB_Printf("Reg 0x%04X = 0x%02X. (In Read mode) \n", RegAdd, Receive);
//		//Eric_H:026411
//		USB_Printf("EEPROM Reg 0x%02X = 0x%02X. (In Read mode) \n", RegAdd, Receive);
//		HAL_Delay(10);
//	}
//	else{
//		//USB_Printf("Reg 0x%04X read fail! (In Read mode) \n", RegAdd);
//		//Eric_H:026411
//		USB_Printf("EEPROM Reg 0x%02X read fail! (In Read mode) \n", RegAdd);
//	}
//
//	return (Receive); // return the value
//}


//Eric_H:026411
uint8_t I2Cdevice_Read(uint8_t I2Cdevice_Read_Addr, uint16_t RegAdd){

	uint8_t Receive = 0; // read back register setting value and initial value is 0
	uint8_t Temp[2] ;

	Temp[1] =  RegAdd;
	Temp[0] =  RegAdd >> 8;


	if(HAL_I2C_Master_Transmit(&hi2c1, I2Cdevice_Read_Addr,Temp ,2, 10000)== HAL_OK){
		//USB_Printf("Reg 0x%04X write OK! (In Read mode) \n", RegAdd);
		HAL_Delay(1);
	}
	else{
		USB_Printf("Reg 0x%04X write Fail! for I2C device of 0x%02X address (In Read mode) \n", RegAdd, I2Cdevice_Read_Addr);
	}

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready

	if(HAL_I2C_Master_Receive(&hi2c1, I2Cdevice_Read_Addr, (uint8_t*)&Receive, 1, 10000) == HAL_OK){
		USB_Printf("Reg 0x%04X = 0x%02X for I2C device of 0x%02X address\n", RegAdd, Receive, I2Cdevice_Read_Addr);
		HAL_Delay(10);
	}
	else{
		USB_Printf("Reg 0x%04X read fail! (In Read mode) for I2C device of 0x%02X address\n", RegAdd, I2Cdevice_Read_Addr);
	}

	return (Receive); // return the value
}

//Eric_H:026414
uint8_t I2Cdevice_Read8(uint8_t I2Cdevice_Read_Addr, uint8_t RegAdd){

	uint8_t Receive = 0; // read back register setting value and initial value is 0
	uint8_t Temp[1] ;

	Temp[0] =  RegAdd;


	if(HAL_I2C_Master_Transmit(&hi2c1, I2Cdevice_Read_Addr,Temp ,1, 10000)== HAL_OK){
		//USB_Printf("Reg 0x%04X write OK! (In Read mode) \n", RegAdd);
		HAL_Delay(1);
	}
	else{
		USB_Printf("Reg 0x%04X write Fail! for I2C device of 0x%02X address (In Read mode) \n", RegAdd, I2Cdevice_Read_Addr);
	}

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready

	if(HAL_I2C_Master_Receive(&hi2c1, I2Cdevice_Read_Addr, (uint8_t*)&Receive, 1, 10000) == HAL_OK){

		USB_Printf("Reg 0x%02X = 0x%02X for I2C device of 0x%02X address\n", RegAdd, Receive, I2Cdevice_Read_Addr);
		HAL_Delay(10);
	}
	else{
		USB_Printf("Reg 0x%02X read fail! for I2C device of 0x%02X address\n", RegAdd, I2Cdevice_Read_Addr);
	}

	return (Receive); // return the value
}

//Eric_H:026416
//bool I2Cdevice_Read8N(uint8_t I2Cdevice_Read_Addr, uint8_t RegAdd, uint8_t count, uint8_t *data)
//bool I2Cdevice_Read8N(uint8_t I2Cdevice_Read_Addr, uint8_t RegAdd, uint8_t count)
uint8_t * I2Cdevice_Read8N(uint8_t I2Cdevice_Read_Addr, uint8_t RegAdd, uint8_t count)
{
	uint8_t Receive[16] = {0}; // read back register setting value and initial value is 0
	uint8_t Temp[1] ;
	Temp[0] =  RegAdd;
	
	if(HAL_I2C_Master_Transmit(&hi2c1, I2Cdevice_Read_Addr, Temp, 1, 10000)== HAL_OK){
		//USB_Printf("Reg 0x%04X write OK! (In Read mode) \n", RegAdd);
		HAL_Delay(1);
	}
	else{
		USB_Printf("Reg 0x%04X write Fail! for I2C device of 0x%02X address (In Read mode) \n", RegAdd, I2Cdevice_Read_Addr);
	}

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready

	if(HAL_I2C_Master_Receive(&hi2c1, I2Cdevice_Read_Addr, (uint8_t*)&Receive, count, 10000) == HAL_OK){
		USB_Printf("Read from Reg 0x%02X:\n", RegAdd);
		for (uint8_t i=0; i<count; i++) {
			USB_Printf("0x%02X ", Receive[i]);
			HAL_Delay(10);
		}
		//return true;
		//return *Receive;
		return (uint8_t*)&Receive;
	}
	else{
		USB_Printf("Reg 0x%02X read fail! for I2C device of 0x%02X address\n", RegAdd, I2Cdevice_Read_Addr);
	}

//	return (Receive); // return the value
}
//Eric_H:026421
uint8_t * I2Cdevice_Read8N_PT(uint8_t I2Cdevice_Read_Addr, uint8_t RegAdd, uint8_t count, uint8_t *Receive)
{
	uint8_t Temp[1] ;
	Temp[0] =  RegAdd;
	
	if(HAL_I2C_Master_Transmit(&hi2c1, I2Cdevice_Read_Addr, Temp, 1, 10000)== HAL_OK){
		HAL_Delay(1);
	}
	else{
		USB_Printf("Reg 0x%04X write Fail! for I2C device of 0x%02X address (In Read mode) \n", RegAdd, I2Cdevice_Read_Addr);
	}

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready

	if(HAL_I2C_Master_Receive(&hi2c1, I2Cdevice_Read_Addr, Receive, count, 10000) == HAL_OK){
		USB_Printf("Read from Reg 0x%02X:\n", RegAdd);
		//for (uint8_t i=0; i<count; i++) {
		//	USB_Printf("0x%02X ", Receive[i]);
		//	HAL_Delay(10);
		//}
		return Receive;
	}
	else{
		USB_Printf("Reg 0x%02X read fail! for I2C device of 0x%02X address\n", RegAdd, I2Cdevice_Read_Addr);
	}

	return Receive; // return the value
}


//Eric_H:026414
void I2Cdevice_Read8_NULL(uint8_t I2Cdevice_Read_Addr){

	uint8_t Temp[1] ;

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready

	if(HAL_I2C_Master_Receive(&hi2c1, I2Cdevice_Read_Addr, (uint8_t*)Temp, 1, 10000) == HAL_OK){

		USB_Printf("Dummy read of Reg NULL is 0x%02X for I2C device of 0x%02X address\n", *Temp, I2Cdevice_Read_Addr);
		HAL_Delay(10);
	}
	else{
		USB_Printf("Read fail! for I2C device of 0x%02X address\n", I2Cdevice_Read_Addr);
	}


}

//Eric_H:026412
//uint8_t * LT6911C_i2c_rd(uint8_t lt6911c_read_addr, uint16_t reg, uint8_t n)
uint8_t LT6911C_i2c_rd(uint8_t lt6911c_read_addr, uint16_t reg, uint8_t n) // <--- Temp
{
	//uint8_t Receive[n]; // read back register setting value and initial value is 0
	uint8_t Receive[16]; // read back register setting value and initial value is 0
	uint8_t buf[2] = { 0xFF, reg >> 8};
	uint8_t reg_addr = reg & 0xFF;

	if(HAL_I2C_Master_Transmit(&hi2c1, lt6911c_read_addr, buf , 2, 10000)== HAL_OK){
		////USB_Printf("\nBank REG 0x%02X%02X write OK! (In Read mode) \n", buf[0], buf[1]);
		HAL_Delay(1);
	}
	else{
		USB_Printf("Bank REG 0x%02X%02X write Fail! for LT6911C of 0x%02X address (In Read mode) \n", buf[0], buf[1], lt6911c_read_addr);
	}
	
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready
	
	if(HAL_I2C_Master_Transmit(&hi2c1, lt6911c_read_addr, (uint8_t*)&reg_addr, 1, 10000)== HAL_OK){
		////USB_Printf("Addr REG 0x%02X write OK! (In Read mode) \n", reg_addr);
		HAL_Delay(1);
	}
	else{
		USB_Printf("Addr REG 0x%02X write Fail! for LT6911C of 0x%02X address (In Read mode) \n", reg_addr, lt6911c_read_addr);
	}
	
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY); //waiting I2C ready
	
	//if(HAL_I2C_Master_Receive(&hi2c1, LT6911C_Read_Addr, (uint8_t*)&Receive, n, 10000) == HAL_OK){
	if(HAL_I2C_Master_Receive(&hi2c1, lt6911c_read_addr, (uint8_t*)Receive, n, 10000) == HAL_OK){
		//USB_Printf("Reg 0x%04X = 0x%02X. (In Read mode) for I2C device of 0x%02X address\n", RegAdd, Receive, I2Cdevice_Read_Addr);
		HAL_Delay(10);
		for(uint8_t i=0; i<n; i++){
			USB_Printf(" 0x%02X ", Receive[i]);
		}
	}
	else{
		USB_Printf("Reg 0x%04X read fail! (In Read mode) for I2C device of 0x%02X address\n", reg, LT6911C_Read_Addr);
	}

	//return Receive; // return the value
	return *Receive; // return the value  <--- Temp
	
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

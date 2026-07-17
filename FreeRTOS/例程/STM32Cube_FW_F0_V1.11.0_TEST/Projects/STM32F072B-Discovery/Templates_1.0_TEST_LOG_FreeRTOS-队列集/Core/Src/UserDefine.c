/**
  ******************************************************************************
  * @file    UserDefine.c
  * @brief   This file provides code for User define subroutines
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 INT Technology. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  * opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "UserDefine.h"

/*--------------------------------------------------------------------------------------
 * Function: PC_CMD_ChkSum
 * Para: CMD_Array
 * Return: boolean
 * Description:
 * 1. Used to calculate the command check sum
 * 2. If check sum is correct then return true, opposite returning false
 * 3. PC CMD Format reference the Command format define guide
--------------------------------------------------------------------------------------*/
bool PC_CMD_ChkSum(uint8_t *ptr){
	uint8_t CalSum = 0; // Calculation sum
	uint8_t DataCount = 7; // used to store the command data quantity
	char i = 0;

	//Calculating Data
	for(i = 0; i < DataCount; i++){
		CalSum += *ptr;
		ptr++;
	}

	if(CalSum == *ptr) return true;
	else return false;
}

/*--------------------------------------------------------------------------------------
 * Function: UnPackRead
 * Para: uint8_t *ptr (the pointer of the receive data buffer)
 * Return: uint8_t Receive
 * Description:
 * 1. Read command subroutine
 *
--------------------------------------------------------------------------------------*/
uint8_t UnPackRead(uint8_t *ptr){
	uint16_t RegAdd = 0; // register of module
	uint8_t Receive = 0;

	RegAdd = *(ptr+4) << 8;
	RegAdd |= *(ptr+5);

	// Receive = Module_Read(RegAdd);
	Receive = Module_Read_L011(RegAdd);
	return (Receive);
}

//Eric_H:026411
uint8_t UnPackRead_2EEPROM(uint8_t *ptr){
	uint8_t RegAdd = 0; // register of module
	uint8_t Receive = 0;

	//RegAdd = *(ptr+5);
	//Eric_H:026414
	RegAdd = *(ptr+4);

	Receive = EEROM_Read(RegAdd);
	return (Receive);
}
uint8_t UnPackRead_2I2Cdevice(uint8_t *ptr){
	uint16_t RegAdd = 0; // register of module
	uint8_t Receive = 0;

	RegAdd = *(ptr+4) << 8;
	RegAdd |= *(ptr+5);

	Receive = I2Cdevice_Read(*(ptr+3), RegAdd);
	return (Receive);
}

//Eric_H:026416
bool UnPackRead_2I2Cdevice8N(uint8_t *ptr){
	// if (I2Cdevice_Read8N(*(ptr+3), *(ptr+4), *(ptr+5), (ptr+6))) return true;
	// else return false;
	return true;
}

//Eric_H:026412
uint8_t UnPackRead_2LT6911C(uint8_t *ptr){
	uint16_t RegAdd = 0; // register of module
	uint8_t *Receive;

	RegAdd = *(ptr+4) << 8;
	RegAdd |= *(ptr+5);

	*Receive = LT6911C_i2c_rd(*(ptr+3), RegAdd, *(ptr+6));
	return (*Receive);
}

//Eric_H:026420
uint8_t * UnPackRead_ECX343EN(uint8_t *ptr){
	uint8_t cmd[2];
	cmd[1] = *(ptr+3);

	cmd[0]=0x80; cmd[1]=0x01;
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);

	cmd[0]=0x81; cmd[1]=*(ptr+3);
// bool I2Cdevice_Write8N(uint8_t I2Cdevice_Write_Addr, uint8_t RegAdd, uint8_t count, uint8_t *data)
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);

//bool I2Cdevice_Read8N(uint8_t I2Cdevice_Read_Addr, uint8_t RegAdd, uint8_t count)
	uint8_t receive[2];

	// *receive = I2Cdevice_Read8N_PT(0x50, 0x01, 2, receive); 
	I2Cdevice_Read8N_PT(0x50, 0x01, 2, receive); 
	
	//USB_Printf("\n\nThe receive is pointer is 0x%02X\n", receive);
	USB_Printf("\nThe REG of ECX343EN @ 0x%02X is: 0x%02X\n\n", *(ptr+3), *(receive+1));
	
	cmd[0]=0x80; cmd[1]=0x00;
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	
	return receive;
}


/*--------------------------------------------------------------------------------------
 * Function: UnPackWrite
 * Para: uint8_t *ptr (the pointer of the receive data buffer)
 * Return: true: success, false: write fail
 * Description:
 * 1. nWrite command subroutine
 * 2.
--------------------------------------------------------------------------------------*/
bool UnPackWrite(uint8_t *ptr){
	uint16_t RegAdd = 0; // register of module
	uint8_t CMD = 0; // command of module

	RegAdd = *(ptr+4) << 8;
	RegAdd |= *(ptr+5);
	CMD = *(ptr+6);
//	if(Module_Write(RegAdd, CMD)) return true;
//	else return false;

	EEFlash_Table_Write(RegAdd, (uint16_t)CMD);
	
		if(Module_Write_L011(RegAdd, CMD,I2C_WRITE)) return true;
	else return false;
//	USB_Printf("Reg 0x%04X write 0x%02X OK! \n", RegAdd, CMD);
//	return true;
}

//Eric_H:026411
bool UnPackWrite_2EEPROM(uint8_t *ptr){
	uint8_t RegAdd = 0; // register of module
	uint8_t CMD = 0; // command of module

	//RegAdd = *(ptr+5);
	//CMD = *(ptr+6);
	//Eric_H:026414
	RegAdd = *(ptr+4);
	CMD = *(ptr+5);
	if(EEROM_Write(RegAdd, CMD)) return true;
	else return false;
}

bool UnPackWrite_2I2Cdevice(uint8_t *ptr){
	uint16_t RegAdd = 0; // register of module
	uint8_t CMD = 0; // command of module

	RegAdd = *(ptr+4) << 8;
	RegAdd |= *(ptr+5);
	CMD = *(ptr+6);
	if (I2Cdevice_Write(*(ptr+3), RegAdd, CMD)) return true;
	else return false;
}

//Eric_H:026414
bool UnPackWrite_2I2Cdevice8N(uint8_t *ptr){
	if (I2Cdevice_Write8N(*(ptr+3), *(ptr+4), *(ptr+5), (ptr+6))) return true;
	else return false;
}

//Eric_H:026412
bool UnPackWrite_2LT6911C(uint8_t *ptr){
	uint16_t RegAdd = 0; // register of module
	bool Receive;

	RegAdd = *(ptr+4) << 8;
	RegAdd |= *(ptr+5);

	Receive = LT6911C_i2c_wr(*(ptr+3), RegAdd, *(ptr+6), (ptr+7));
	return Receive;
}

//Eric_H:026420
bool UnPackWrite_ECX343EN(uint8_t *ptr){
	uint8_t cmd[]={ *(ptr+3), *(ptr+4) };

// bool I2Cdevice_Write8N(uint8_t I2Cdevice_Write_Addr, uint8_t RegAdd, uint8_t count, uint8_t *data)
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);

	USB_Printf("\n\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", *(ptr+3), *(ptr+4));
	//return true;
}


//Eric_H:026422
const uint8_t PanelContOnSeqTable_RGB[] = {
		0x00, 0x00,
		0x01, 0x00,
		0x02, 0x8c,
		0x03, 0x58,
		0x04, 0x03,
		0x05, 0x00,
		0x06, 0x00,
		0x07, 0x20,
		0x08, 0x00,
		0x09, 0x00,
		0x0A, 0x00,
		0x0B, 0x00,
		0x0C, 0x00,
		0x0D, 0x10,
		0x0E, 0x44,
		0x0F, 0x00,
		0x10, 0x0F,
		0x11, 0x04,
		0x12, 0x01,
		0x13, 0x5e,

		0x3E, 0x21,

		0x43, 0x40,
		0x44, 0x29,
		0x45, 0xD9,
		0x46, 0x02,
		0x47, 0x71,

		0x49, 0x18,

		0x4B, 0x22,
		0x4C, 0x72,
		0x4D, 0x71,
		0x4E, 0x04,
		0x4F, 0x7d,
		0x50, 0x18,
		0x51, 0x22,
		0x52, 0x73,
		0x53, 0x72,
		0x54, 0x04,
		0x55, 0x7d,
		0x56, 0x18,

		0x5A, 0x10,

		0x61, 0x11,
		0x62, 0x03,
		0x63, 0x00,
		0x64, 0xed,
		0x65, 0x06,
		0x66, 0x00,
		0x67, 0x17,
		0x68, 0xf9,
		0x69, 0x00,
		0x6A, 0x09,
		0x6B, 0xe7,
		0x6C, 0x00,
		0x6D, 0xee,
		0x6E, 0xf5,
		0x6F, 0x10,
		0x70, 0x19,
		0x71, 0x11,
		0x72, 0x06,
		0x73, 0x16,

		0x75, 0x0e,
		0x76, 0x00,
		0x77, 0x00,
		0x78, 0x00,
		0x79, 0x00,
		0x7A, 0x00,
		0x7B, 0x00,
		0x7C, 0x00,
		0x7D, 0x00,

		0x80, 0x00,
		0x81, 0x00,
		0x82, 0x00,
		0x83, 0x03,
		0x84, 0x84,
		0x85, 0x14,

		0x89, 0x04,
		0x8A, 0xe2,
		0x8B, 0xda,

		0x8E, 0x00,

		0xBF, 0x08,

		0xF2, 0x01,
		0xF3, 0x38,
		0xF4, 0x40,
		0xF5, 0xe1,
		0xF6, 0x42,
		0xF7, 0x71,
		0xF8, 0xe1,
		0xF9, 0x42,
		0xFA, 0x71,
		0xFB, 0xe1,
		0xFC, 0x70,
		0xFD, 0x00,
		0xFE, 0xff,

		0x10, 0x0A,
		0x12, 0x01,
		0x13, 0x5e,
		0x10, 0x0F,
};

//Eric_H:026422
const uint8_t PanelContGIllumiOnSeqTable_RGB[] = {
		0x10, 0x1f,
		0x4E, 0x00,
		0x4F, 0x19,
		0x54, 0x0F,
		0x55, 0xFF,
		0x77, 0x01,
		0x78, 0x04,
		0x79, 0xcb,
		0x7A, 0x09,
		0x7B, 0xC3,
		0x7D, 0xFA,
		0x8B, 0x5A,
};

#define PANEL_PS_MASK			(uint8_t)0xF8
// PS0
#define PANEL_PS0_ON			(uint8_t)0x00		// 0 = Power Save On
#define PANEL_PS0_OFF			(uint8_t)0x01		// 1 = Power Save Off
// PS1
#define PANEL_PS1_ON			(uint8_t)0x00		// 0 = Power Save On
#define PANEL_PS1_OFF			(uint8_t)0x02		// 1 = Power Save Off
// PS2
#define PANEL_PS2_ON			(uint8_t)0x00		// 0 = Power Save On
#define PANEL_PS2_OFF			(uint8_t)0x04		// 1 = Power Save Off
//Eric_H:026422
const uint8_t PanelContPsOffSeqTable[] = {
		0x00, PANEL_PS0_OFF,										// PS OFF
		0x00, PANEL_PS1_OFF | PANEL_PS0_OFF,					// PS OFF
		0x00, PANEL_PS2_OFF | PANEL_PS1_OFF | PANEL_PS0_OFF,	// PS OFF
};
//Eric_H:026422
const uint8_t PanelContDSPON_SeqTable[] = {
		0x0b, 0x07,
		0x0c, 0x0f,
		0x10, 0x00,
		0xab, 0x07,
		0x36, 0xff,
		0x2a, 0x01,
		0x2b, 0xff,
		0x2c, 0xff,
		0x2d, 0xff,
		0x2e, 0xff,
};
//Eric_H:026422
const uint8_t PanelContBIST_SeqTable[] = {
		0x0b, 0x07,
		0x0c, 0x0f,
		0x10, 0x00,
		0xab, 0x07,
		0x36, 0xaa,
		0x10, 0x01,
		0xc6, 0x03,
		0x04, 0x05,
		0xd2, 0xff,
		0xd3, 0xff,
		0xd4, 0xff,
		0x36, 0xff,
		0x37, 0x01,
};
	
//Eric_H:026422
bool UnPackWrite_ECX343EN_Power_On_SEQ(uint8_t *ptr){
	uint8_t i;
	uint8_t cmd[2];

// bool I2Cdevice_Write8N(uint8_t I2Cdevice_Write_Addr, uint8_t RegAdd, uint8_t count, uint8_t *data)
	
	cmd[0] = 0x82;
	cmd[1] = 0x00;
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	USB_Printf("\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", cmd[0], cmd[1]);
	
	#define  ECX343EN_Power_On_CmdData_Sets ( sizeof(PanelContOnSeqTable_RGB) / 2 )
	for ( i = 0; i < ECX343EN_Power_On_CmdData_Sets; i++ ){
	//cmd[0] = 0x82;
	//cmd[1] = 0x00;
	//I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	
	cmd[0] = PanelContOnSeqTable_RGB [ i * 2 ];
	cmd[1] = PanelContOnSeqTable_RGB [ i * 2 + 1 ];
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	USB_Printf("\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", cmd[0], cmd[1]);
	HAL_Delay(150);
	}
	
	HAL_Delay(500);
	
	#define  ECX343EN_GIllumiOnSeq_CmdData_Sets ( sizeof(PanelContGIllumiOnSeqTable_RGB) / 2 )
	for ( i = 0; i < ECX343EN_GIllumiOnSeq_CmdData_Sets; i++ ){
	//cmd[0] = 0x82;
	//cmd[1] = 0x00;
	//I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	
	cmd[0] = PanelContGIllumiOnSeqTable_RGB [ i * 2 ];
	cmd[1] = PanelContGIllumiOnSeqTable_RGB [ i * 2 + 1 ];
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	USB_Printf("\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", cmd[0], cmd[1]);
	HAL_Delay(150);
	}
	
	HAL_Delay(500);
	
	#define  ECX343EN_PsOffSeq_CmdData_Sets ( sizeof(PanelContPsOffSeqTable) / 2 )
	for ( i = 0; i < ECX343EN_PsOffSeq_CmdData_Sets; i++ ){
	//cmd[0] = 0x82;
	//cmd[1] = 0x00;
	//I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	
	cmd[0] = PanelContPsOffSeqTable [ i * 2 ];
	cmd[1] = PanelContPsOffSeqTable [ i * 2 + 1 ];
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	USB_Printf("\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", cmd[0], cmd[1]);
	HAL_Delay(150);
	}

	//HAL_Delay(2000);
	//
	//#define  ECX343EN_BIST_CmdData_Sets ( sizeof(PanelContBIST_SeqTable) / 2 )
	//for ( i = 0; i < ECX343EN_BIST_CmdData_Sets; i++ ){
	////cmd[0] = 0x82;
	////cmd[1] = 0x00;
	////I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	//
	//cmd[0] = PanelContBIST_SeqTable [ i * 2 ];
	//cmd[1] = PanelContBIST_SeqTable [ i * 2 + 1 ];
	//I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	//USB_Printf("\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", cmd[0], cmd[1]);
	//HAL_Delay(500);
	//}
	//return true;
}


//Eric_H:026422
bool UnPackWrite_ECX343EN_DSPON_SEQ(uint8_t *ptr){
	uint8_t i;
	uint8_t cmd[2];

	cmd[0] = 0x82;
	cmd[1] = 0x00;
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	USB_Printf("\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", cmd[0], cmd[1]);
	
	#define  ECX343EN_DSPON_CmdData_Sets ( sizeof(PanelContDSPON_SeqTable) / 2 )
	for ( i = 0; i < ECX343EN_DSPON_CmdData_Sets; i++ ){
	
	cmd[0] = PanelContDSPON_SeqTable [ i * 2 ];
	cmd[1] = PanelContDSPON_SeqTable [ i * 2 + 1 ];
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	USB_Printf("\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", cmd[0], cmd[1]);
	HAL_Delay(500);
	}
	//return true;
}


//Eric_H:026422
bool UnPackWrite_ECX343EN_BIST_SEQ(uint8_t *ptr){
	uint8_t i;
	uint8_t cmd[2];

	cmd[0] = 0x82;
	cmd[1] = 0x00;
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	USB_Printf("\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", cmd[0], cmd[1]);
	
	#define  ECX343EN_BIST_CmdData_Sets ( sizeof(PanelContBIST_SeqTable) / 2 )
	for ( i = 0; i < ECX343EN_BIST_CmdData_Sets; i++ ){
	
	cmd[0] = PanelContBIST_SeqTable [ i * 2 ];
	cmd[1] = PanelContBIST_SeqTable [ i * 2 + 1 ];
	I2Cdevice_Write8N(0x50, 0x01, 2, cmd);
	USB_Printf("\nSet REG of ECX343EN @ 0x%02X to: 0x%02X\n", cmd[0], cmd[1]);
	HAL_Delay(500);
	}
	//return true;
}


//Eric_H:026418
#include "LT6911C_Flash.bin"
//Eric_H:026415
bool UnPackWrite_Flash_LT6911C(uint8_t *ptr){
	
	uint8_t time;
	
	uint8_t i2c_enable_disable[1]={I2C_ENABLE};
	//uint8_t data[16]={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };	// 2)
	//uint8_t data[16]={ 0x02, 0x2B, 0x00, 0x90, 0x02, 0x77, 0xE0, 0xFD, 0x7F, 0x0C, 0x7E, 0xC2, 0x12, 0x33, 0xB2, 0x90 };	// 2)
	
				//for (time=0; time<2; time++){
				for (time=0; time<1; time++){
	//Step 1: Config LT6911C into flash mode
	//LT6911C_i2c_wr(LT6911C_Write_Addr, I2C_EN_REG, 1, i2c_enable_disable);	// 1)
	//USB_Printf("\nStep 1: P1\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);						// 2)
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x86);
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
	////USB_Printf("Step 1: P2\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5B, 0x00);						// 3)
	//USB_Printf("Step 1: P3\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5C, 0x00);						// 4)
	//USB_Printf("Step 1: P4\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5D, 0x00);						// 5)
	//USB_Printf("Step 1: P5\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x83);						// 6)
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
	//USB_Printf("Step 1: P6\n");
	//USB_Printf("\nStep 1: P1\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);						// 2)
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x86);
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
	////USB_Printf("Step 1: P2\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5B, 0x00);						// 3)
	//USB_Printf("Step 1: P3\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5C, 0x00);						// 4)
	//USB_Printf("Step 1: P4\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5D, 0x00);						// 5)
	//USB_Printf("Step 1: P5\n");
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x83);						// 6)
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
	//USB_Printf("Step 1: P6\n");
	uint8_t earse_flash[] = { 0x82, 0xc2, 0x82, 0x86, 0x82, 0x92, 0x51, 0x00, 0xc0, 0x82, 0x01 };
	USB_Printf("\nStep 1: P1/Config Register\n");
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x80EE, 1, earse_flash+ 0);
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x805A, 1, earse_flash+ 1);
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x805E, 1, earse_flash+ 2);
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x8058, 1, earse_flash+ 3);
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x8059, 1, earse_flash+ 4);
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x805A, 1, earse_flash+ 5);
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x805A, 1, earse_flash+ 6);
	USB_Printf("\nStep 1: P2/Earse Chip\n");
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x805A, 1, earse_flash+ 7);
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x805A, 1, earse_flash+ 8);
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x805A, 1, earse_flash+ 9);
	LT6911C_i2c_wr(LT6911C_Write_Addr, 0x805A, 1, earse_flash+10);
	
	HAL_Delay(4000);

				//for (time=0; time<2; time++){
	LT6911C_i2c_rd(LT6911C_Read_Addr, 0x9002, 1);							// 7)
	//HAL_Delay(100);
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x02, 0xDF);						// 8)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x02, 0xFF);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0xFF, 0x80);						// 9)
	USB_Printf("Step 1: P7\n");
	LT6911C_i2c_wr(LT6911C_Write_Addr, I2C_EN_REG, 1, i2c_enable_disable);	//10)
	USB_Printf("\nStep 1: P8\n");
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x86);						//11)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x86);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
	USB_Printf("Step 1: P9\n");

	//#define  pack 18
	#define  pack 16
	#define  rest_bytes ( sizeof(LT6911C_Flash_data1) % pack )
	#define  packs ( ( sizeof(LT6911C_Flash_data1) / sizeof(LT6911C_Flash_data1[0] ) + pack - 1 ) / pack )
	uint16_t i;

	//Step 2: Iteration for HEX data sending
	//for ( i = 0; i < 831; i++ ){
	for ( i = 0; i < packs-1; i++ ){
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5E, 0xEF);						// 1)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0xA2);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x58, 0x01);
	USB_Printf("Step 2: P1\n");
    
	//I2Cdevice_Write8N(LT6911C_Write_Addr, 0x59, 16, (LT6911C_Flash_data1+i*18));			// 2)
	I2Cdevice_Write8N(LT6911C_Write_Addr, 0x59, 16, (LT6911C_Flash_data1+i*pack));			// 2)
	USB_Printf("Step 2: P2\n");
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5B, 0x00);						// 3) 
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5C, LT6911C_Flash_data1[i*18 + 16]);           	//<--Page Number
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5D, LT6911C_Flash_data1[i*18 + 17]);           	//<--Pack Number
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5C, i/16);						//<--Page Number
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5D, (i%16)*16);				 	//<--Pack Number
	USB_Printf("Step 2: P3\n");
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5E, 0xE0);						// 4)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x92);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x86);						//<--Last SEG of last PAG
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
	USB_Printf("Step 2: P4, i is %4d\n", i);
	}
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5E, 0xEF);						// 1)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0xA2);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x58, 0x01);
	USB_Printf("Step 2: P1\n");
	//I2Cdevice_Write8N(LT6911C_Write_Addr, 0x59, 14, data);				// 2)
	//I2Cdevice_Write8N(LT6911C_Write_Addr, 0x59, 14, (LT6911C_Flash_data1+i*18));			// 2)
	//I2Cdevice_Write8N(LT6911C_Write_Addr, 0x59, 14, (LT6911C_Flash_data1+i*pack));		// 2)
	I2Cdevice_Write8N(LT6911C_Write_Addr, 0x59, rest_bytes, (LT6911C_Flash_data1+i*pack));	// 2)
	USB_Printf("Step 2: P2\n");
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5B, 0x00);						// 3) 
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5C, LT6911C_Flash_data1[i*18 + 16]);           	//<--Page Number
    //I2Cdevice_Write8(LT6911C_Write_Addr, 0x5D, LT6911C_Flash_data1[i*18 + 17]);           	//<--Pack Number
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5C, i/16);						//<--Page Number
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5D, (i%16)*16);					//<--Pack Number
	USB_Printf("Step 2: P3\n");
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5E, 0xE0);						// 4)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x92);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x8A);						//<--Last SEG of last PAG
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
	USB_Printf("Step 2: P4, i is %4d\n", i);
				}

	
	//HAL_Delay(500);
	HAL_Delay(4000);
	
	
	LT6911C_i2c_rd(LT6911C_Read_Addr, 0x9002, 1);							// 7)
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x02, 0xDF);						// 8)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x02, 0xFF);
	I2Cdevice_Write8(LT6911C_Write_Addr, 0xFF, 0x80);						// 9)
	
	LT6911C_i2c_wr(LT6911C_Write_Addr, I2C_EN_REG, 1, i2c_enable_disable);	//10)
    
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x86);						//11)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5E, 0x6F);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0xA2);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);						// 3) 
    
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5B, 0x00);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5C, 0x00);           	//<--Segm Number
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5D, 0x00);           	//<--Segm Number
    
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x92);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x58, 0x01);
    I2Cdevice_Read8N(LT6911C_Read_Addr, 0x5F, 16);
    
}

//Eric_H:026418
bool UnPackWrite_EraseFlash_LT6911C(uint8_t *ptr){

	uint8_t time;
	uint8_t i2c_enable_disable[1]={I2C_ENABLE};
	
				for (time=0; time<1; time++){
	LT6911C_i2c_wr(LT6911C_Write_Addr, I2C_EN_REG, 1, i2c_enable_disable);	// 1)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);						// 2)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5E, 0xC0);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x58, 0x00);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x59, 0x51);						// 3)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x92);						// 4)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);						// 5)
    
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x86);						// 6)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0xC2);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);

	
	HAL_Delay(4000);
	
	
	LT6911C_i2c_rd(LT6911C_Read_Addr, 0x9002, 1);							// 7)
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x02, 0xDF);						// 8)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x02, 0xFF);
	I2Cdevice_Write8(LT6911C_Write_Addr, 0xFF, 0x80);						// 9)
	
	LT6911C_i2c_wr(LT6911C_Write_Addr, I2C_EN_REG, 1, i2c_enable_disable);	//10)
    
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x86);						//11)
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5E, 0x6F);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0xA2);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);						// 3) 
    
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5B, 0x00);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5C, 0x00);           	//<--Segm Number
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5D, 0x00);           	//<--Segm Number
    
	I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x92);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x5A, 0x82);
    I2Cdevice_Write8(LT6911C_Write_Addr, 0x58, 0x01);
    I2Cdevice_Read8N(LT6911C_Read_Addr, 0x5F, 16);
				}
}

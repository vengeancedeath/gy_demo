/*
 * qspi.h
 *
 *  Created on: Nov 15, 2024
 *      Author: junjie.lai
 */

#ifndef QSPI_QSPI_H_
#define QSPI_QSPI_H_
#include "main.h"
#define ROW 640
#define COLUMN 480


void oled_QSPI_reset(void);

/* --------QSPIģʽ-------------- */	
void oled_QSPI_init(void);
void QSPI_delay(uint16_t j);


void qspi_send_message(uint8_t *date ,uint16_t length);
void QSPI_send_image(const uint8_t image[][ROW]);


	
#endif /* QSPI_QSPI_H_ */

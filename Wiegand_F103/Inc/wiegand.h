/*
 * wiegand.h
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: dima
 */

#ifndef WIEGAND_H_
#define WIEGAND_H_

#include "main.h"


void ReadD0(); // interrupt
void ReadD1(); // interrupt
uint8_t wig_available();
//uint8_t DoWiegandConversion();
uint32_t GetCardId(volatile uint32_t *codehigh, volatile uint32_t *codelow, uint8_t bitlength);
uint32_t getCode();
int16_t getWiegandType();

#endif /* WIEGAND_H_ */

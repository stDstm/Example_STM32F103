/*
 * delay_micros.h
 *
 *  Created on: 29 сент. 2019 г.
 *      Author: dima
 */

#ifndef DELAY_MICROS_H_
#define DELAY_MICROS_H_

#include "main.h"

void DWT_Init(void);
void delay_micros(uint32_t us);

#endif /* DELAY_MICROS_H_ */

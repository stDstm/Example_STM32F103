/*
 * rxb6.h
 *
 *  Created on: 27 февр. 2020 г.
 *      Author: dima
 */

#ifndef RXB6_H_
#define RXB6_H_

#include "main.h"

#define SIZE_ARRAY 66

#define SIGNAL_PORT	    GPIOA          // ваш порт
#define SIGNAL_PIN  	GPIO_PIN_15    // ваш пин

#define HCS_TE          400
#define HCS_Te2_3       600         // HCS_TE * 3 / 2


__STATIC_INLINE void DWT_Init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
}

__STATIC_INLINE uint32_t micros(void)
{
	return  DWT->CYCCNT / (SystemCoreClock / 1000000U);
}


void RX433_Int();

#endif /* RXB6_H_ */

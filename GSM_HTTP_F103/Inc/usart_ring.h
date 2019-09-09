/*
 * usart_ring.h
 *
 *  Created on: 19 авг. 2019 г.
 *      Author: dima
 */

#ifndef USART_RING_H_
#define USART_RING_H_

#include "main.h"

/////////////////// GSM USART /////////////////////
#define GPRS_RX_BUFFER_SIZE 1024

#if (GPRS_RX_BUFFER_SIZE > 256)
	typedef uint16_t gsm_rx_buffer_index_t;
#else
	typedef uint8_t gsm_rx_buffer_index_t;
#endif

uint16_t gsm_available(void);
uint16_t gsm_read(void);

/////////////////// DEBUG USART ///////////////////// можно удалить после отладки всего
#define DBG_RX_BUFFER_SIZE 64

#if (DBG_RX_BUFFER_SIZE > 256)
	typedef uint16_t dbg_rx_buffer_index_t;
#else
	typedef uint8_t dbg_rx_buffer_index_t;
#endif

uint16_t dbg_available(void);
uint16_t dbg_read(void);


#endif /* USART_RING_H_ */



/* USER CODE BEGIN USART1_IRQn 0 */
	/*if((huart1.Instance->SR & USART_SR_RXNE) != RESET)
	{
		uint8_t rbyte = (uint8_t)(huart1.Instance->DR & (uint8_t)0x00FF); // читает байт из регистра
		gsm_rx_buffer_index_t i = (uint16_t)(gsm_rx_buffer_head + 1) % GPRS_RX_BUFFER_SIZE;

		if(i != gsm_rx_buffer_tail)
		{
			gsm_rx_buffer[gsm_rx_buffer_head] = rbyte;
			gsm_rx_buffer_head = i;
		}
	}

	return;*/
/* USER CODE END USART1_IRQn 0 */


/* USER CODE BEGIN USART3_IRQn 0 */
	/*if((huart3.Instance->SR & USART_SR_RXNE) != RESET)
	{
		uint8_t rbyte = (uint8_t)(huart3.Instance->DR & (uint8_t)0x00FF); // читает байт из регистра
		dbg_rx_buffer_index_t i = (uint16_t)(dbg_rx_buffer_head + 1) % DBG_RX_BUFFER_SIZE;

		if(i != dbg_rx_buffer_tail)
		{
			dbg_rx_buffer[dbg_rx_buffer_head] = rbyte;
			dbg_rx_buffer_head = i;
		}
	}

	return;*/
/* USER CODE END USART3_IRQn 0 */













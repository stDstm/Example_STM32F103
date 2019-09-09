/*
 * usart_ring.c
 *
 *  Created on: 19 авг. 2019 г.
 *      Author: dima
 */

#include "usart_ring.h"

extern UART_HandleTypeDef huart1;

/////////////////// GSM USART /////////////////////
volatile gsm_rx_buffer_index_t gsm_rx_buffer_head = 0;
volatile gsm_rx_buffer_index_t gsm_rx_buffer_tail = 0;
uint8_t gsm_rx_buffer[GPRS_RX_BUFFER_SIZE] = {0,};

void clear_gsm_buff()
{
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
	gsm_rx_buffer_head = 0;
	gsm_rx_buffer_tail = 0;
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	//gsm_rx_buffer_head = gsm_rx_buffer_tail;
}

uint16_t gsm_available(void)
{
	return ((uint16_t)(GPRS_RX_BUFFER_SIZE + gsm_rx_buffer_head - gsm_rx_buffer_tail)) % GPRS_RX_BUFFER_SIZE;
}

uint16_t gsm_read(void)
{
	if(gsm_rx_buffer_head == gsm_rx_buffer_tail)
	{
		return -1;
	}
	else
	{
		unsigned char c = gsm_rx_buffer[gsm_rx_buffer_tail];
		gsm_rx_buffer_tail = (gsm_rx_buffer_index_t)(gsm_rx_buffer_tail + 1) % GPRS_RX_BUFFER_SIZE;
		return c;
	}
}

/////////////////// DEBUG USART //////////////////// можно удалить после отладки всего

volatile dbg_rx_buffer_index_t dbg_rx_buffer_head = 0;
volatile dbg_rx_buffer_index_t dbg_rx_buffer_tail = 0;
uint8_t dbg_rx_buffer[DBG_RX_BUFFER_SIZE] = {0,};

uint16_t dbg_available(void)
{
	return ((uint16_t)(DBG_RX_BUFFER_SIZE + dbg_rx_buffer_head - dbg_rx_buffer_tail)) % DBG_RX_BUFFER_SIZE;
}

uint16_t dbg_read(void)
{
	if(dbg_rx_buffer_head == dbg_rx_buffer_tail)
	{
		return -1;
	}
	else
	{
		uint8_t c = dbg_rx_buffer[dbg_rx_buffer_tail];
		dbg_rx_buffer_tail = (dbg_rx_buffer_index_t)(dbg_rx_buffer_tail + 1) % DBG_RX_BUFFER_SIZE;
		return c;
	}
}






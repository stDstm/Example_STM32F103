/*
 * usart_dma_rx_buf.c
 *
 *  Created on: 24 авг. 2019 г.
 *      Author: dima
 */

#include "main.h"
#include "usart_dma_rx_buf.h"

extern UART_HandleTypeDef huart1;
extern uint8_t rx_buffer[RX_BUFFER_SIZE];
uint16_t write_index = 0;
uint16_t read_index = 0;

uint16_t usart1_available(void)
{
	uint16_t get_count = __HAL_DMA_GET_COUNTER(huart1.hdmarx);
	write_index = get_count * (0 - RX_BUFFER_SIZE) / RX_BUFFER_SIZE + RX_BUFFER_SIZE;
	return write_index - read_index;
}

uint16_t usart1_read(void)
{
	uint8_t res = rx_buffer[read_index];
	read_index++;

	if(read_index == RX_BUFFER_SIZE)
	{
		read_index = 0;
		HAL_UART_Receive_DMA(&huart1, (uint8_t*)rx_buffer, RX_BUFFER_SIZE);
	}

	return res;
}

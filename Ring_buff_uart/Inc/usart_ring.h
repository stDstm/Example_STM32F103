/*
 * usart_ring.h
 *
 *  Created on: 19 авг. 2019 г.
 *      Author: dima
 */

#ifndef USART_RING_H_
#define USART_RING_H_

#include "main.h"

#define MYUART huart1 // задефайнить USART

#define UART_RX_BUFFER_SIZE 128 // указать размер входного буфера

#if (UART_RX_BUFFER_SIZE > 256)
	typedef uint16_t rx_buffer_index_t;
#else
	typedef uint8_t rx_buffer_index_t;
#endif

uint16_t uart_available(void);
int16_t uart_read(void);
void clear_uart_buff();

#endif /* USART_RING_H_ */


// это нужно добавить в файл stm32f1xx_it.c

/* USER CODE BEGIN Includes */
//#include "usart_ring.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/*extern volatile rx_buffer_index_t rx_buffer_head;
extern volatile rx_buffer_index_t rx_buffer_tail;
extern unsigned char rx_buffer[UART_RX_BUFFER_SIZE];*/
/* USER CODE END PV */


// добавить в обработчик прерывания USART'а

/* USER CODE BEGIN USART1_IRQn 0 */
	/*if((MYUART.Instance->SR & USART_SR_RXNE) != RESET)
	{
		uint8_t rbyte = (uint8_t)(MYUART.Instance->DR & (uint8_t)0x00FF); // читает байт из регистра
		rx_buffer_index_t i = (uint16_t)(rx_buffer_head + 1) % UART_RX_BUFFER_SIZE;

		if(i != rx_buffer_tail)
		{
			rx_buffer[rx_buffer_head] = rbyte;
			rx_buffer_head = i;
		}
	}

	return;*/
/* USER CODE END USART1_IRQn 0 */
















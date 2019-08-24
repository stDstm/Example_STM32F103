/*
 * usart_dma_rx_buf.h
 *
 *  Created on: 24 авг. 2019 г.
 *      Author: dima
 */

#ifndef USART_DMA_RX_BUF_H_
#define USART_DMA_RX_BUF_H_

#define RX_BUFFER_SIZE 20

uint16_t usart1_available(void);
uint16_t usart1_read(void);

#endif /* USART_DMA_RX_BUF_H_ */

/*
 * print_bits.h
 *
 *  Created on: 23 апр. 2020 г.
 *      Author: dima
 */

#ifndef PRINT_BITS_H_
#define PRINT_BITS_H_

#define SIZE_BUF_BITS	32

void print_bits(char *buf_bits, uint32_t dig, uint8_t num_bits)
{
	//uint8_t c = ' ';   // delimiter character
	memset(buf_bits, 0, SIZE_BUF_BITS);

	for(uint8_t i = 0; i < num_bits; i++)
	{
		buf_bits[i] = (dig & (1 << (num_bits - 1 - i))) > 0 ? '1' : '0';
		//buf_bits[i] = b;
		//if (i < (num_bits - 1) && ((num_bits - i - 1) % 4 == 0 )) HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, 1000); // print a separator at every 4 bits
	}
}

#endif /* PRINT_BITS_H_ */


//////////////////////////////////////////// в main.c //////////////////////////////////////////////////

// #include "print_bits.h"

/*
char buf_bits[SIZE_BUF_BITS] = {0,}; // буфер в который запишутся биты

// первый аргумент - буфер в который запишутся биты, второй - число, которое нужно перевести в бинарное, третий - размерность числа - 8 бит, 16 бит, 32 бита
print_bits(buf_bits, 254, 8);

HAL_UART_Transmit(&huart1, (uint8_t*)buf_bits, strlen(buf_bits), 100);
HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 1000);
*/



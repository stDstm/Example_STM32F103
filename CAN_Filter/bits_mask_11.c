
//#include <windows.h>
#include <string.h>
#include <stdio.h>  
#include <stdlib.h>
#include <stdint.h>

#define SIZE_BUF_BITS	33


void print_bits(char *buf_bits, uint32_t dig)
{
	memset(buf_bits, 0, SIZE_BUF_BITS);
	
	uint8_t num_bits = 16;
	uint8_t b = 5;

	for(uint8_t i = 0; i < (num_bits - 5); i++)
	{
		buf_bits[i] = (dig & (1 << (num_bits - 1 - b))) > 0 ? '1' : '0';
		b++;
	}
}


int main(int argc, char *argv[])  
{  
	if(argc != 3) 
	{
		printf("Error arg\n");
		exit(0);
	}
	
	uint32_t ID = strtoul(argv[1], NULL, 0);
	uint32_t Mask = strtoul(argv[2], NULL, 0);
	
	char buf_bits[SIZE_BUF_BITS] = {0,}; // буфер в который запишутся биты

	print_bits(buf_bits, ID);

	printf("\n HEX        BIN\n");
	printf("-------------------------------------\n");
	printf("0x%04X  %s    ID - первый регистр фильтра\n", ID, buf_bits);
	
	print_bits(buf_bits, Mask);

	printf("0x%04X  %s    Mask - второй регистр фильтра\n\n", Mask, buf_bits);

	printf("ID которые будут приняты\n");
	printf("-------------------------\n");
 
 
	for(uint32_t i = 0; i < 0x800; i++)
	{
		if((i & Mask) == ID) 
		{
			print_bits(buf_bits, i);
			printf("0x%04X  %s\n", i, buf_bits);
		}
	}

	return 0;
}


// gcc -Wextra -Wall bits_mask_11.c -o bits_mask_11

// ./bits_mask_11 0x0100 0x07F8


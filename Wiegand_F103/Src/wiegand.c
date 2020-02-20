/*
 * wiegand.c
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: dima
 */

#include "wiegand.h"

volatile uint32_t cardTempHigh = 0;
volatile uint32_t cardTemp = 0;
volatile uint32_t lastWiegand = 0;
uint32_t code = 0;
volatile int16_t bitCount = 0;
int16_t wiegandType = 0;



uint32_t getCode()
{
	return code;
}

int16_t getWiegandType()
{
	return wiegandType;
}
/*
uint8_t wig_available()
{
	uint8_t ret;
	//HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);   // отключаем прерывания WIG

	ret = DoWiegandConversion();

	//__HAL_GPIO_EXTI_CLEAR_IT(D0);    // очищаем бит EXTI_PR
	//__HAL_GPIO_EXTI_CLEAR_IT(D0_Pin);    // очищаем бит EXTI_PR
	//NVIC_ClearPendingIRQ(EXTI15_10_IRQn);  // очищаем бит NVIC_ICPRx
	//HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);    // включаем прерывания WIG

	return ret;
}
*/

void ReadD0()
{
	bitCount++;				// Increament bit count for Interrupt connected to D0

	if (bitCount > 31)			// If bit count more than 31, process high bits
	{
		cardTempHigh |= ((0x80000000 & cardTemp) >> 31);	//	shift value to high bits
		cardTempHigh <<= 1;
		cardTemp <<= 1;
	}
	else
	{
		cardTemp <<= 1;		// D0 represent binary 0, so just left shift card data
	}

	lastWiegand = HAL_GetTick();	// Keep track of last wiegand bit received
}

void ReadD1()
{
	bitCount++;				// Increment bit count for Interrupt connected to D1

	if(bitCount > 31)			// If bit count more than 31, process high bits
	{
		cardTempHigh |= ((0x80000000 & cardTemp) >> 31);	// shift value to high bits
		cardTempHigh <<= 1;
		cardTemp |= 1;
		cardTemp <<= 1;
	}
	else
	{
		cardTemp |= 1;			// D1 represent binary 1, so OR card data with 1 then
		cardTemp <<= 1;		// left shift card data
	}

	lastWiegand = HAL_GetTick();	// Keep track of last wiegand bit received
}

uint32_t GetCardId(volatile uint32_t *codehigh, volatile uint32_t *codelow, uint8_t bitlength)
{

	if(bitlength == 26)								// EM tag
	{
		return (*codelow & 0x1FFFFFE) >> 1;
	}

	if(bitlength == 34)								// Mifare
	{
		*codehigh = *codehigh & 0x03;				// only need the 2 LSB of the codehigh
		*codehigh <<= 30;							// shift 2 LSB to MSB
		*codelow >>= 1;
		return *codehigh | *codelow;
	}

	return *codelow;								// EM tag or Mifare without parity bits
}

uint8_t translateEnterEscapeKeyPress(uint8_t originalKeyPress)
{
	switch(originalKeyPress)
	{
		case 0x0b:        // 11 or * key
			return 0x0d;  // 13 or ASCII ENTER

		case 0x0a:        // 10 or # key
			return 0x1b;  // 27 or ASCII ESCAPE

		default:
			return originalKeyPress;
	}
}

uint8_t wig_available()
{
	uint32_t cardID;
	uint32_t time_wig = HAL_GetTick();

	if((time_wig - lastWiegand) > 25) // if no more signal coming through after 25ms
	{
		if((bitCount == 24) || (bitCount == 26) || (bitCount == 32) || (bitCount == 34) || (bitCount == 8) || (bitCount == 4)) // bitCount for keypress=4 or 8, Wiegand 26=24 or 26, Wiegand 34=32 or 34
		{
			cardTemp >>= 1; // shift right 1 bit to get back the real value - interrupt done 1 left shift in advance

			if(bitCount > 32) // bit count more than 32 bits, shift high bits right to make adjustment
			{
				cardTempHigh >>= 1;
			}

			if(bitCount == 8)		// keypress wiegand with integrity
			{
				// 8-bit Wiegand keyboard data, high nibble is the "NOT" of low nibble
				// eg if key 1 pressed, data=E1 in binary 11100001 , high nibble=1110 , low nibble = 0001
				uint8_t highNibble = (cardTemp & 0xf0) >> 4;
				uint8_t lowNibble = (cardTemp & 0x0f);
				wiegandType = bitCount;
				bitCount = 0;
				cardTemp = 0;
				cardTempHigh = 0;

				if(lowNibble == (~highNibble & 0x0f))		// check if low nibble matches the "NOT" of high nibble.
				{
					code = (int16_t)translateEnterEscapeKeyPress(lowNibble);
					return 1;
				}
				else
				{
					lastWiegand = time_wig;
					bitCount = 0;
					cardTemp = 0;
					cardTempHigh = 0;
					return 0;
				}

				// TODO: Handle validation failure case!
			}
			else if(4 == bitCount)
			{
				// 4-bit Wiegand codes have no data integrity check so we just
				// read the LOW nibble.
				code = (int16_t)translateEnterEscapeKeyPress(cardTemp & 0x0000000F);

				wiegandType = bitCount;
				bitCount = 0;
				cardTemp = 0;
				cardTempHigh = 0;

				return 1;
			}
			else		// wiegand 26 or wiegand 34
			{
				cardID = GetCardId(&cardTempHigh, &cardTemp, bitCount);
				wiegandType = bitCount;
				bitCount = 0;
				cardTemp = 0;
				cardTempHigh = 0;
				code = cardID;
				return 1;
			}
		}
		else
		{
			// well time over 25 ms and bitCount !=8 , !=26, !=34 , must be noise or nothing then.
			lastWiegand = time_wig;
			bitCount = 0;
			cardTemp = 0;
			cardTempHigh = 0;
			return 0;
		}
	}
	else
	{
		return 0;
	}
}




















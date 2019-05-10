/*
 * ds18b20.c
 *
 *  Created on: 5 мая 2019 г.
 *      Author: istarik.ru
 */


#include "ds18b20.h"

extern TIM_HandleTypeDef htim4;
extern volatile uint8_t flag;

#define MY_PORT GPIOB
#define MY_PIN  GPIO_PIN_5

uint32_t DELAY_WAIT_CONVERT = DELAY_T_CONVERT;

void DWT_Init(void) // инициализируем счетчик тактов DWT (с его помощью будем отсчитывать микросекунды)
{
    SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
	DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
}

void delay_us(uint32_t us)
{
    uint32_t us_count_tic =  us * (SystemCoreClock / 1000000); // делим системную частоту (72мГц) и получаем 72 такта за одну микросекунду
    DWT->CYCCNT = 0U; // обнуляем счётчик
    while(DWT->CYCCNT < us_count_tic);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim4)
	{
		HAL_TIM_Base_Stop_IT(htim);
		flag = 2;
	}
}

void Init_ds18b20(DS18B20_Resolution resolution)
{
	DWT_Init();
	setResolution(resolution);
}

void reset()
{
	HAL_GPIO_WritePin(MY_PORT, MY_PIN, GPIO_PIN_RESET);
	delay_us(DELAY_RESET);
	HAL_GPIO_WritePin(MY_PORT, MY_PIN, GPIO_PIN_SET);
	delay_us(DELAY_RESET);
}

uint8_t getDevider(DS18B20_Resolution resolution)
{
	uint8_t devider;
	switch (resolution)
	{
		case DS18B20_Resolution_9_bit:
			devider = 8;
			break;
		case DS18B20_Resolution_10_bit:
			devider = 4;
			break;
		case DS18B20_Resolution_11_bit:
			devider = 2;
			break;
		case DS18B20_Resolution_12_bit:
		default:
			devider = 1;
			break;
	}

	return devider;
}

void writeBit(uint8_t bit)
{
	HAL_GPIO_WritePin(MY_PORT, MY_PIN, GPIO_PIN_RESET);
	delay_us(bit ? DELAY_WRITE_1 : DELAY_WRITE_0);
	HAL_GPIO_WritePin(MY_PORT, MY_PIN, GPIO_PIN_SET);
	delay_us(bit ? DELAY_WRITE_1_PAUSE : DELAY_WRITE_0_PAUSE);
}

void writeByte(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		writeBit(data >> i & 1);
		delay_us(DELAT_PROTECTION);
	}
}

uint8_t readBit()
{
	uint8_t bit = 0;
	HAL_GPIO_WritePin(MY_PORT, MY_PIN, GPIO_PIN_RESET);
	delay_us(DELAY_READ_SLOT);
	HAL_GPIO_WritePin(MY_PORT, MY_PIN, GPIO_PIN_SET);
	delay_us(DELAY_BUS_RELAX);
	bit = (HAL_GPIO_ReadPin(MY_PORT, MY_PIN) ? 1 : 0);
	delay_us(DELAY_READ_PAUSE);
	return bit;
}

void ds18b20_getTemperature()
{
	reset();
	writeByte(SKIP_ROM);
	writeByte(CONVERT_T);
	__HAL_TIM_SET_AUTORELOAD(&htim4, DELAY_WAIT_CONVERT);
	HAL_TIM_Base_Start_IT(&htim4); // запускаем таймер
}

int16_t resTemperature()
{
	reset();
	writeByte(SKIP_ROM);
	writeByte(READ_SCRATCHPAD);
	int16_t data = 0;

	for(uint8_t i = 0; i < 16; i++)
	{
		data += (int16_t)readBit() << i;
	}

	return data;
}

void setResolution(DS18B20_Resolution resolution)
{
	reset();
	writeByte(SKIP_ROM);
	writeByte(WRITE_SCRATCHPAD);
	writeByte(TH_REGISTER);
	writeByte(TL_REGISTER);
	writeByte(resolution);
	DELAY_WAIT_CONVERT = DELAY_T_CONVERT / getDevider(resolution);
}

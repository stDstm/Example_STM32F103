/*
 * epm_24c32.c
 *
 *  Created on: 22 мая 2019 г.
 *      Author: dima
 */

#include "epm_24c32.h"

void Write_mem(uint16_t addr, char *wstr, uint16_t len)
{
	uint16_t cycle = len / SIZEBLOCK;
	uint32_t status = 0;

	for(uint16_t i = 0; i < cycle + 1; i++)
	{
		if(len > SIZEBLOCK)
		{
			status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)wstr, SIZEBLOCK, HAL_MAX_DELAY);
			wstr = wstr + SIZEBLOCK;
			len = len - SIZEBLOCK;
			addr = addr + SIZEBLOCK;
		}
		else
		{
			status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)wstr, len, HAL_MAX_DELAY);
		}

		if(status != HAL_OK)
		{
			char str[32] = {0,};
			snprintf(str, 32, "HAL_I2C_Mem_Write");
			I2C_Error(str, status);
		}

		for(uint8_t i = 0; i < 100; i++) // ждём пока eeprom доделает свои делишки и ответит что готова.
		{
			status = HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 1, 100);

			if(status == HAL_OK) break;

			if(i > 99)
			{
				char str[32] = {0,};
				snprintf(str, 32, "HAL_I2C_IsDeviceReady");
				I2C_Error(str, status);
			}
		}
	}
}

void Read_mem(uint16_t addr, char *rstr, uint16_t len)
{
	uint32_t status = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)rstr, len, HAL_MAX_DELAY);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read");
		I2C_Error(str, status);
	}
}

/*void Write_mem_single_block(uint16_t addr, char *wstr, uint16_t len) // записывает в eeprom один блок (32 байта), перед чтением нужна пауза
{
	uint16_t status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)wstr, len, HAL_MAX_DELAY);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read");
		I2C_Error(str, status);
	}
}*/



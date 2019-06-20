/*
 * ds3231.c
 *
 *  Created on: 23 мая 2019 г.
 *      Author: dima
 */

#include "main.h"
#include "ds3231.h"
#include "string.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
uint8_t sec = 0, min = 0, hour = 0, day = 0, date = 0, month = 0, year = 0;

void Set_time_data()
{
	uint8_t data[7] = {0,};
	data[0] = sec;
	data[1] = min;
	data[2] = hour;
	data[3] = day;
	data[4] = date;
	data[5] = month;
	data[6] = year;

	uint32_t status = HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, 7, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read");
		I2C_Error(str, status);
	}
}


void Read_time_data()
{
	char readBuf[7] = {0,};
	uint32_t status = HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, (uint8_t*)readBuf, 7, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read");
		I2C_Error(str, status);
	}

	sec = readBuf[0];
	min = readBuf[1];
	hour = readBuf[2];
	day = readBuf[3];
	date = readBuf[4];
	month = readBuf[5];
	year = readBuf[6];
}

///////////////////////////////

uint8_t bcd2dec(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd % 16);
}

uint8_t dec2bcd(uint8_t dec)
{
    return ((dec / 10) * 16) + (dec % 10);
}

/////////////////////////////

uint8_t readSeconds()
{
	return bcd2dec(sec);
}

uint8_t readMinutes()
{
	return bcd2dec(min);
}

uint8_t readHours()
{
	return bcd2dec(hour);
}

uint8_t readDate()
{
	return bcd2dec(date);
}

uint8_t readMonth()
{
	return bcd2dec(month);
}

uint8_t readYear()
{
	return bcd2dec(year);
}

uint8_t readDay()
{
	return bcd2dec(day);
}

/////////////////////////////////

void setSeconds(uint8_t new_sec)
{
	sec = dec2bcd(new_sec);
}

void setMinutes(uint8_t new_min)
{
	min = dec2bcd(new_min);
}

void setHour(uint8_t new_hour)
{
	hour = dec2bcd(new_hour);
}

void setDate(uint8_t new_date)
{
	date = dec2bcd(new_date);
}

void setMonth(uint8_t new_month)
{
	month = dec2bcd(new_month);
}

void setYear(uint8_t new_year)
{
	year = dec2bcd(new_year);
}

void setDay(uint8_t new_day)
{
	day = new_day;
}

/////////////////////////////////////

float Read_temp_ds3213()
{
	char readBuf[2] = {0,};
	uint32_t status = HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDR, 0x11, I2C_MEMADD_SIZE_8BIT, (uint8_t*)readBuf, 2, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read");
		I2C_Error(str, status);
	}

	return ((((uint8_t)readBuf[0] << 8) | (uint8_t)readBuf[1]) >> 6) / 4.0;
}




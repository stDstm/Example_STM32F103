/*
 * ds3231.c
 *
 *  Created on: 23 мая 2019 г.
 *      Author: dima
 */

#include "main.h"
#include "ds3231.h"
#include "string.h"
#include "stdio.h"

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

//////////////////////////////////////////

void setOutput(uint8_t enable)
{
	uint8_t reg = 0;
	uint32_t status = HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDR, REG_CON, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read_sOt");
		I2C_Error(str, status);
	}

	reg &= ~(1 << 2);
	reg |= (enable << 2);

	status = HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDR, REG_CON, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Wr_sOt");
		I2C_Error(str, status);
	}
}

void setSQWRate(uint8_t rate)
{
	uint8_t reg = 0;
	uint32_t status = HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDR, REG_CON, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read_SQW");
		I2C_Error(str, status);
	}

	reg &= ~(3 << 3);
	reg |= (rate << 3);

	status = HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDR, REG_CON, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Wr_SQW");
		I2C_Error(str, status);
	}
}


void enable32KHz(uint8_t enable)
{
	uint8_t reg = 0;
	uint32_t status = HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDR, REG_STATUS, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read_32k");
		I2C_Error(str, status);
	}

	reg &= ~(1 << 3);
	reg |= (enable << 3);

	status = HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDR, REG_STATUS, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Wr_32k");
		I2C_Error(str, status);
	}
}

///////////////////////////////////////// ALARM 1 //////////////////////////////////////////////////

void armAlarm1(uint8_t armed)
{
    uint8_t value;

    uint32_t status = HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDR, REG_CON, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&value, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read_AL");
		I2C_Error(str, status);
	}

    if(armed)
    {
        value |= 0b00000001;
    }
    else
    {
        value &= 0b11111110;
    }

    status = HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDR, REG_CON, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&value, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Wr_AL");
		I2C_Error(str, status);
	}
}


void clearAlarm1(void)
{
    uint8_t value;

    uint32_t status = HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDR, REG_STATUS, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&value, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Read_CA");
		I2C_Error(str, status);
	}

    value &= 0b11111110;

    status = HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDR, REG_STATUS, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&value, 1, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Wr_CA");
		I2C_Error(str, status);
	}
}


void setAlarm1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, uint8_t armed)
{
    second = dec2bcd(second);
    minute = dec2bcd(minute);
    hour = dec2bcd(hour);
    dydw = dec2bcd(dydw);

    switch(mode)
    {
        case EVERY_SECOND:
            second |= 0b10000000;
            minute |= 0b10000000;
            hour |= 0b10000000;
            dydw |= 0b10000000;
            break;

        case MATCH_S:
            second &= 0b01111111;
            minute |= 0b10000000;
            hour |= 0b10000000;
            dydw |= 0b10000000;
            break;

        case MATCH_M_S:
            second &= 0b01111111;
            minute &= 0b01111111;
            hour |= 0b10000000;
            dydw |= 0b10000000;
            break;

        case MATCH_H_M_S:
            second &= 0b01111111;
            minute &= 0b01111111;
            hour &= 0b01111111;
            dydw |= 0b10000000;
            break;

        case MATCH_DT_H_M_S:
            second &= 0b01111111;
            minute &= 0b01111111;
            hour &= 0b01111111;
            dydw &= 0b01111111;
            break;

        case MATCH_DY_H_M_S:
            second &= 0b01111111;
            minute &= 0b01111111;
            hour &= 0b01111111;
            dydw &= 0b01111111;
            dydw |= 0b01000000;
            break;
    }

    uint8_t data[4] = {0,};
    data[0] = second;
    data[1] = minute;
    data[2] = hour;
    data[3] = dydw;

    uint32_t status = HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDR, REG_ALARM_1, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, 4, 1000);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Wr_SA");
		I2C_Error(str, status);
	}

    armAlarm1(armed);
    clearAlarm1();
}














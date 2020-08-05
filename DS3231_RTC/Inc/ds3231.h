/*
 * ds3231.h
 *
 *  Created on: 23 мая 2019 г.
 *      Author: dima
 */

#ifndef DS3231_H_
#define DS3231_H_

#include "main.h"

#define DS3231_ADDR (0x68 << 1) // адрес устройства - 0x68

#define REG_CON		0x0e
#define REG_STATUS	0x0f

#define SQW_RATE_1		0
#define SQW_RATE_1K		1
#define SQW_RATE_4K		2
#define SQW_RATE_8K		3

#define OUTPUT_SQW		0
#define OUTPUT_INT		1

#define REG_ALARM_1     (0x07)


typedef enum
{
    EVERY_SECOND   = 0b00001111,
    MATCH_S        = 0b00001110,
    MATCH_M_S      = 0b00001100,
    MATCH_H_M_S    = 0b00001000,
    MATCH_DT_H_M_S = 0b00000000,
    MATCH_DY_H_M_S = 0b00010000
} DS3231_alarm1_t;


void Read_time_data();
void Set_time_data();
void I2C_Error(char *er, uint32_t status);

uint8_t readSeconds();
uint8_t readMinutes();
uint8_t readHours();
uint8_t readDate();
uint8_t readMonth();
uint8_t readYear();
uint8_t readDay();

void setSeconds(uint8_t new_sec);
void setMinutes(uint8_t new_min);
void setHour(uint8_t new_hour);
void setDate(uint8_t new_date);
void setMonth(uint8_t new_month);
void setYear(uint8_t new_year);
void setDay(uint8_t new_day);

float Read_temp_ds3213();

void setOutput(uint8_t enable);
void setSQWRate(uint8_t rate);
void enable32KHz(uint8_t enable);


void armAlarm1(uint8_t armed);
void clearAlarm1(void);
void setAlarm1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, uint8_t armed);


#endif /* DS3231_H_ */











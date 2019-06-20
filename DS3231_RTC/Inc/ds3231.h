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

#endif /* DS3231_H_ */

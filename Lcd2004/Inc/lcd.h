/*
 * lcd.h
 *
 *  Created on: 7 окт. 2018 г.
 *      Author: dima
 */

#ifndef LCD_H_
#define LCD_H_

#define LCD_ADDR (0x3F << 1) // адрес должен быть сдвинут

#define E_high()    LCD_WriteByteI2CLCD(control |= 0x04)  // стробирующий импульс Е в 1
#define E_low()  LCD_WriteByteI2CLCD(control &= ~0x04) // стробирующий импульс Е в 0
#define RS_high()   LCD_WriteByteI2CLCD(control |= 0x01)  //установка линии RS в 1 - символ
#define RS_low() LCD_WriteByteI2CLCD(control &= ~0x01) //установка линии RS в 0 - команда
#define led_on()   LCD_WriteByteI2CLCD(control |= 0x08)  //включение подсветки
#define led_off()   LCD_WriteByteI2CLCD(control &= ~0x08) //включение подсветки
#define RW_low() LCD_WriteByteI2CLCD(control &= ~0x02) // RW ставим в ноль (запись в дисплей)

void DWT_Init(void);
void LCD_ini(void);
void LCD_Clear(void);
void LCD_SendChar(char ch);
void LCD_String(char* st);
void LCD_SetPos(uint8_t x, uint8_t y);

#endif /* LCD_H_ */


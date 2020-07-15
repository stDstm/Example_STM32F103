/*
 * xpt2046_touch.c
 *
 *  Created on: 5 апр. 2020 г.
 *      Author: dima
 */


#include "main.h"
#include "xpt2046_touch.h"
#include "ILI9341_GFX.h"

#define TOUCH_SCALE_X 320
#define TOUCH_SCALE_Y 240

#define TOUCH_MIN_RAW_X 1500
#define TOUCH_MAX_RAW_X 30000
#define TOUCH_MIN_RAW_Y 2500
#define TOUCH_MAX_RAW_Y 30000

static const uint8_t cmd_read_x = 0x90;
static const uint8_t cmd_read_y = 0xD0;
static const uint8_t zeroes_tx[] = {0x00, 0x00};

// калибровка
/*#include "string.h"
#include "stdio.h"
extern UART_HandleTypeDef huart1;*/


uint8_t ILI9341_TouchGetCoordinates(uint16_t *x, uint16_t *y)
{
	if(HAL_GPIO_ReadPin(IRQ_GPIO_Port, IRQ_Pin) != GPIO_PIN_RESET) return 0;

    TOUCH_CS_SELECT;

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;

	HAL_SPI_Transmit(TOUCH_SPI_PTR, (uint8_t*)&cmd_read_y, 1, 1000);

	uint8_t y_raw[2] = {0,};
	HAL_SPI_TransmitReceive(TOUCH_SPI_PTR, (uint8_t*)zeroes_tx, y_raw, 2, 1000);

	HAL_SPI_Transmit(TOUCH_SPI_PTR, (uint8_t*)&cmd_read_x, 1, 1000);

	uint8_t x_raw[2] = {0,};
	HAL_SPI_TransmitReceive(TOUCH_SPI_PTR, (uint8_t*)zeroes_tx, x_raw, 2, 1000);

	avg_x = (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
	avg_y = (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);

	// калибровка
	/*char buf[64] = {0,};
	snprintf(buf, 64, "ADC_X = %lu, ADC_Y = %lu\n", avg_x, avg_y);
	HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 100);*/

    TOUCH_CS_UNSELECT;

    if(avg_x < TOUCH_MIN_RAW_X) avg_x = TOUCH_MIN_RAW_X;
    if(avg_x > TOUCH_MAX_RAW_X) avg_x = TOUCH_MAX_RAW_X;

    if(avg_y < TOUCH_MIN_RAW_X) avg_y = TOUCH_MIN_RAW_Y;
    if(avg_y > TOUCH_MAX_RAW_Y) avg_y = TOUCH_MAX_RAW_Y;

    *x = (avg_x - TOUCH_MIN_RAW_X) * TOUCH_SCALE_X / (TOUCH_MAX_RAW_X - TOUCH_MIN_RAW_X);
    *y = (avg_y - TOUCH_MIN_RAW_Y) * TOUCH_SCALE_Y / (TOUCH_MAX_RAW_Y - TOUCH_MIN_RAW_Y);

    return 1;
}


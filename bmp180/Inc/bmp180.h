/*
 * bmp180.h
 *
 *  Created on: 20 мая 2019 г.
 *      Author: dima
 */

#ifndef BMP180_H_
#define BMP180_H_

#include "main.h"

#define BMP_ADDRESS (0x77 << 1) // АДРЕС УСТРОЙСТВА - 0x77

#define I2CDEV_DEFAULT_READ_TIMEOUT 1000

#define BMP_RA_AC1_H     0xAA    /* AC1_H */
#define BMP_RA_CONTROL   0xF4    /* CONTROL */
#define BMP_RA_MSB       0xF6    /* MSB */

#define BMP_MODE_TEMPERATURE     0x2E
#define BMP_MODE_PRESSURE_0      0x34
#define BMP_MODE_PRESSURE_1      0x74
#define BMP_MODE_PRESSURE_2      0xB4
#define BMP_MODE_PRESSURE_3      0xF4

void BMP_i2c_init(I2C_HandleTypeDef *hi2c);
void BMP_setControl(uint8_t value);
uint16_t BMP_getMeasurement2(); // 16-bit data
uint32_t BMP_getMeasurement3(); // 24-bit data
uint8_t BMP_getMeasureDelayMilliseconds(uint8_t mode);
float BMP_getTemperatureC();
float BMP_getPressure();
void I2C_Error(char *er, uint32_t status);
void BMP_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout);

#endif /* BMP180_H_ */

/*
 * bmp180.c
 *
 *  Created on: 20 мая 2019 г.
 *      Author: dima
 */

#include "bmp180.h"
#include "string.h"

static uint8_t devAddr = BMP_ADDRESS;
static uint8_t buffer[3];

static int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
static uint16_t ac4, ac5, ac6;
static int32_t b5;
static uint8_t measureMode;

/////////////////////////////////////////////////////////////
static I2C_HandleTypeDef *BMP_hi2c;

void BMP_i2c_init(I2C_HandleTypeDef *hi2c)
{
	BMP_hi2c = hi2c;

    uint8_t buf2[22];
    BMP_readBytes(devAddr, BMP_RA_AC1_H, 22, buf2, 1000);
    ac1 = ((int16_t)buf2[0] << 8) + buf2[1];
    ac2 = ((int16_t)buf2[2] << 8) + buf2[3];
    ac3 = ((int16_t)buf2[4] << 8) + buf2[5];
    ac4 = ((uint16_t)buf2[6] << 8) + buf2[7];
    ac5 = ((uint16_t)buf2[8] << 8) + buf2[9];
    ac6 = ((uint16_t)buf2[10] << 8) + buf2[11];
    b1 = ((int16_t)buf2[12] << 8) + buf2[13];
    b2 = ((int16_t)buf2[14] << 8) + buf2[15];
    mb = ((int16_t)buf2[16] << 8) + buf2[17];
    mc = ((int16_t)buf2[18] << 8) + buf2[19];
    md = ((int16_t)buf2[20] << 8) + buf2[21];
}

void BMP_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout)
{
	uint32_t status = 0;
    uint16_t tout = timeout > 0 ? timeout : I2CDEV_DEFAULT_READ_TIMEOUT;
    status = HAL_I2C_Master_Transmit(BMP_hi2c, devAddr, &regAddr, 1, tout);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Master_Transmit");
		I2C_Error(str, status);
		return;
	}

	status = HAL_I2C_Master_Receive(BMP_hi2c, devAddr, data, length, tout);

	if(status != HAL_OK)
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Master_Receive");
		I2C_Error(str, status);
	}
}

void BMP_setControl(uint8_t value)
{
	uint32_t status = HAL_I2C_Mem_Write(BMP_hi2c, devAddr, BMP_RA_CONTROL, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000);

	if(status == HAL_OK)
	{
		measureMode = value;
	}
	else
	{
		char str[32] = {0,};
		snprintf(str, 32, "HAL_I2C_Mem_Write");
		I2C_Error(str, status);
	}
}

uint16_t BMP_getMeasurement2()
{
	BMP_readBytes(devAddr, BMP_RA_MSB, 2, buffer, 1000);
    return ((uint16_t) buffer[0] << 8) + buffer[1];
}

uint32_t BMP_getMeasurement3()
{
	BMP_readBytes(devAddr, BMP_RA_MSB, 3, buffer, 1000);
    return ((uint32_t)buffer[0] << 16) + ((uint16_t)buffer[1] << 8) + buffer[2];
}

uint8_t BMP_getMeasureDelayMilliseconds(uint8_t mode)
{
    if (mode == 0) mode = measureMode;
    if (measureMode == 0x2E) return 5;
    else if (measureMode == 0x34) return 5;
    else if (measureMode == 0x74) return 8;
    else if (measureMode == 0xB4) return 14;
    else if (measureMode == 0xF4) return 26;
    return 0;
}

uint16_t BMP_getRawTemperature()
{
    if (measureMode == 0x2E) return BMP_getMeasurement2();
    return 0;
}

float BMP_getTemperatureC()
{
    int32_t ut = BMP_getRawTemperature();
    int32_t x1 = ((ut - (int32_t)ac6) * (int32_t)ac5) >> 15;
    int32_t x2 = ((int32_t)mc << 11) / (x1 + md);
    b5 = x1 + x2;
    return (float)((b5 + 8) >> 4) / 10.0f;
}


uint32_t BMP_getRawPressure()
{
    if (measureMode & 0x34) return BMP_getMeasurement3() >> (8 - ((measureMode & 0xC0) >> 6));
    return 0;
}

float BMP_getPressure()
{
    uint32_t up = BMP_getRawPressure();
    uint8_t oss = (measureMode & 0xC0) >> 6;
    int32_t p;
    int32_t b6 = b5 - 4000;
    int32_t x1 = ((int32_t)b2 * ((b6 * b6) >> 12)) >> 11;
    int32_t x2 = ((int32_t)ac2 * b6) >> 11;
    int32_t x3 = x1 + x2;
    int32_t b3 = ((((int32_t)ac1 * 4 + x3) << oss) + 2) >> 2;
    x1 = ((int32_t)ac3 * b6) >> 13;
    x2 = ((int32_t)b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    uint32_t b4 = ((uint32_t)ac4 * (uint32_t)(x3 + 32768)) >> 15;
    uint32_t b7 = ((uint32_t)up - b3) * (uint32_t)(50000UL >> oss);

    if (b7 < 0x80000000)
    {
        p = (b7 << 1) / b4;
    }
    else
    {
        p = (b7 / b4) << 1;
    }
    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    return p + ((x1 + x2 + (int32_t)3791) >> 4);
}





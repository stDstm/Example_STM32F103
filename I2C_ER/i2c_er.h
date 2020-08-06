/*
 * i2c_er.h
 *
 *  Created on: Mar 8, 2020
 *      Author: dima
 */

#ifndef I2C_ER_H_
#define I2C_ER_H_

#define SCL_PIN     GPIO_PIN_8
#define SCL_PORT    GPIOB

#define SDA_PIN     GPIO_PIN_9
#define SDA_PORT    GPIOB



static uint8_t wait_for_gpio_state_timeout(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state, uint32_t timeout)
 {
    uint32_t Tickstart = HAL_GetTick();
    uint8_t ret = 1;

    for(;(state != HAL_GPIO_ReadPin(port, pin)) && (1 == ret);) // Wait until flag is set
    {
        if(timeout != HAL_MAX_DELAY) // Check for the timeout
        {
            if((timeout == 0U) || ((HAL_GetTick() - Tickstart) > timeout)) ret = 0;
        }

        asm("nop");
    }
    return ret;
}


static void I2C_ClearBusyFlagErratum(I2C_HandleTypeDef *hi2c, uint32_t timeout)
{
	// 2.13.7 I2C analog filter may provide wrong value, locking BUSY. STM32F10xx8 STM32F10xxB Errata sheet

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // 1. Clear PE bit.
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_PE);

    //  2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
    HAL_I2C_DeInit(hi2c);

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    GPIO_InitStructure.Pin = SCL_PIN;
    HAL_GPIO_Init(SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = SDA_PIN;
    HAL_GPIO_Init(SDA_PORT, &GPIO_InitStructure);

    // 3. Check SCL and SDA High level in GPIOx_IDR.
    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET);

    wait_for_gpio_state_timeout(SCL_PORT, SCL_PIN, GPIO_PIN_SET, timeout);
    wait_for_gpio_state_timeout(SDA_PORT, SDA_PIN, GPIO_PIN_SET, timeout);

    // 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET);

    // 5. Check SDA Low level in GPIOx_IDR.
    wait_for_gpio_state_timeout(SDA_PORT, SDA_PIN, GPIO_PIN_RESET, timeout);

    // 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET);

    // 7. Check SCL Low level in GPIOx_IDR.
    wait_for_gpio_state_timeout(SCL_PORT, SCL_PIN, GPIO_PIN_RESET, timeout);

    // 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET);

    // 9. Check SCL High level in GPIOx_IDR.
    wait_for_gpio_state_timeout(SCL_PORT, SCL_PIN, GPIO_PIN_SET, timeout);

    // 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET);

    // 11. Check SDA High level in GPIOx_IDR.
    wait_for_gpio_state_timeout(SDA_PORT, SDA_PIN, GPIO_PIN_SET, timeout);

    // 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    //GPIO_InitStructure.Alternate = GPIO_AF4_I2C2; // F4

    GPIO_InitStructure.Pin = SCL_PIN;
    HAL_GPIO_Init(SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = SDA_PIN;
    HAL_GPIO_Init(SDA_PORT, &GPIO_InitStructure);

    // 13. Set SWRST bit in I2Cx_CR1 register.
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_SWRST);
    asm("nop");

    /* 14. Clear SWRST bit in I2Cx_CR1 register. */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_SWRST);
    asm("nop");

    /* 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_PE);
    asm("nop");

    // Call initialization function.
    HAL_I2C_Init(hi2c);
}

#endif /* I2C_ER_H_ */

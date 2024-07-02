#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define I2C2_SCL_PIN GPIO_Pin_10
#define I2C2_SDA_PIN GPIO_Pin_11

#define I2C2_SCL_PinSource GPIO_PinSource10
#define I2C2_SDA_PinSource GPIO_PinSource11

#define I2C2_SCL_AF GPIO_AF_I2C2
#define I2C2_SDA_AF GPIO_AF_I2C2

#define I2C2_GPIO_PORT GPIOB

#define I2C2_GPIO_CLK RCC_AHB1Periph_GPIOB
#define I2C2_CLK RCC_APB1Periph_I2C2

#define I2CNUM I2C2

void I2C2_Init(void);
void I2C2_Write(u8 device, u8 address, u8 data);

#endif

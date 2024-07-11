#ifndef __SPI_H
#define __SPI_H
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

//////////////////////////////////////////////////////////////////////////////////

// SPI 驱动代码

//////////////////////////////////////////////////////////////////////////////////

void SPI1_Init(void);                     // 初始化SPI1口
void SPI1_SetSpeed(u8 SpeedSet);          // 设置SPI1速度
uint8_t SPI1_ReadWriteByte(uint8_t data); // SPI1总线读写一个字节

#endif

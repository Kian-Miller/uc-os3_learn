#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx.h"

// 引脚定义
/*******************************************************/
#define KEY0_PIN GPIO_Pin_4
#define KEY0_GPIO_PORT GPIOE
#define KEY0_GPIO_CLK RCC_AHB1Periph_GPIOE

#define KEY1_PIN GPIO_Pin_3
#define KEY1_GPIO_PORT GPIOE
#define KEY1_GPIO_CLK RCC_AHB1Periph_GPIOE
/*******************************************************/

/** 按键按下标置宏
 * 按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
 * 若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
 */
#define KEY_ON 0
#define KEY_OFF 1

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef *GPIOx, u16 GPIO_Pin);

#endif /* __LED_H */

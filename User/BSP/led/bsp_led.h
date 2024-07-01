#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"

// 引脚定义
/*******************************************************/
#define LED2_PIN GPIO_Pin_6
#define LED2_GPIO_PORT GPIOA
#define LED2_GPIO_CLK RCC_AHB1Periph_GPIOA

#define LED3_PIN GPIO_Pin_7
#define LED3_GPIO_PORT GPIOA
#define LED3_GPIO_CLK RCC_AHB1Periph_GPIOA

/** 控制LED灯亮灭的宏，
 * LED低电平亮，设置ON=0，OFF=1
 * 若LED高电平亮，把宏设置成ON=1 ，OFF=0 即可
 */
#define ON 0
#define OFF 1

/* 带参宏，可以像内联函数一样使用 */
#define LED2(a)                                 \
	if (a)                                      \
		GPIO_SetBits(LED2_GPIO_PORT, LED2_PIN); \
	else                                        \
		GPIO_ResetBits(LED2_GPIO_PORT, LED2_PIN)

#define LED3(a)                                 \
	if (a)                                      \
		GPIO_SetBits(LED3_GPIO_PORT, LED3_PIN); \
	else                                        \
		GPIO_ResetBits(LED3_GPIO_PORT, LED3_PIN)

/* 直接操作寄存器的方法控制IO */
#define digitalHi(p, i) \
	{                   \
		p->BSRRL = i;   \
	} // 设置为高电平
#define digitalLo(p, i) \
	{                   \
		p->BSRRH = i;   \
	} // 输出低电平
#define digitalToggle(p, i) \
	{                       \
		p->ODR ^= i;        \
	} // 输出反转状态

/* 定义控制IO的宏 */
#define LED2_TOGGLE digitalToggle(LED2_GPIO_PORT, LED2_PIN)
#define LED2_OFF digitalHi(LED2_GPIO_PORT, LED2_PIN)
#define LED2_ON digitalLo(LED2_GPIO_PORT, LED2_PIN)

#define LED3_TOGGLE digitalToggle(LED3_GPIO_PORT, LED3_PIN)
#define LED3_OFF digitalHi(LED3_GPIO_PORT, LED3_PIN)
#define LED3_ON digitalLo(LED3_GPIO_PORT, LED3_PIN)

void LED_GPIO_Config(void);

#endif /* __LED_H */

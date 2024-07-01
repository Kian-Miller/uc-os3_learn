#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"

// ���Ŷ���
/*******************************************************/
#define LED2_PIN GPIO_Pin_6
#define LED2_GPIO_PORT GPIOA
#define LED2_GPIO_CLK RCC_AHB1Periph_GPIOA

#define LED3_PIN GPIO_Pin_7
#define LED3_GPIO_PORT GPIOA
#define LED3_GPIO_CLK RCC_AHB1Periph_GPIOA

/** ����LED������ĺ꣬
 * LED�͵�ƽ��������ON=0��OFF=1
 * ��LED�ߵ�ƽ�����Ѻ����ó�ON=1 ��OFF=0 ����
 */
#define ON 0
#define OFF 1

/* ���κ꣬��������������һ��ʹ�� */
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

/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define digitalHi(p, i) \
	{                   \
		p->BSRRL = i;   \
	} // ����Ϊ�ߵ�ƽ
#define digitalLo(p, i) \
	{                   \
		p->BSRRH = i;   \
	} // ����͵�ƽ
#define digitalToggle(p, i) \
	{                       \
		p->ODR ^= i;        \
	} // �����ת״̬

/* �������IO�ĺ� */
#define LED2_TOGGLE digitalToggle(LED2_GPIO_PORT, LED2_PIN)
#define LED2_OFF digitalHi(LED2_GPIO_PORT, LED2_PIN)
#define LED2_ON digitalLo(LED2_GPIO_PORT, LED2_PIN)

#define LED3_TOGGLE digitalToggle(LED3_GPIO_PORT, LED3_PIN)
#define LED3_OFF digitalHi(LED3_GPIO_PORT, LED3_PIN)
#define LED3_ON digitalLo(LED3_GPIO_PORT, LED3_PIN)

void LED_GPIO_Config(void);

#endif /* __LED_H */

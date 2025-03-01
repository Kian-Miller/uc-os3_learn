#ifndef __DEBUG_USART_H
#define __DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>

// 引脚定义
/*******************************************************/
#define DEBUG_USART USART1

/* 不同的串口挂载的总线不一样，时钟使能函数也不一样，移植时要注意
 * 串口1和6是      RCC_APB2PeriphClockCmd
 * 串口2/3/4/5是    RCC_APB1PeriphClockCmd
 */

#define DEBUG_USART_CLK RCC_APB2Periph_USART1
#define DEBUG_USART_BAUDRATE 115200 // 串口波特率

#define DEBUG_USART_RX_GPIO_PORT GPIOA
#define DEBUG_USART_RX_GPIO_CLK RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_PIN GPIO_Pin_10
#define DEBUG_USART_RX_AF GPIO_AF_USART1
#define DEBUG_USART_RX_SOURCE GPIO_PinSource10

#define DEBUG_USART_TX_GPIO_PORT GPIOA
#define DEBUG_USART_TX_GPIO_CLK RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_PIN GPIO_Pin_9
#define DEBUG_USART_TX_AF GPIO_AF_USART1
#define DEBUG_USART_TX_SOURCE GPIO_PinSource9

#define DEBUG_USART_IRQHandler USART1_IRQHandler
#define DEBUG_USART_IRQ USART1_IRQn
/************************************************************/

void Debug_USART_Config(void);
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t ch);
void Usart_SendString(USART_TypeDef *pUSARTx, char *str);

void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch);

#endif /* __USART1_H */

/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                           (c) Copyright 2009-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : JJL
*                 EHS
*                 DC
*********************************************************************************************************
*/

#ifndef __APP_CFG_H__
#define __APP_CFG_H__

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/
// 是否向STM32内部写入数据
#define STM32_FLASH_WRITE 1u

#define APP_CFG_SERIAL_EN DEF_DISABLED // Modified by fire ????? DEF_ENABLED??

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define APP_TASK_START_PRIO 2

#define APP_TASK_LED2_PRIO 5
#define APP_TASK_LED3_PRIO 5
#define APP_TASK_KEY_PRIO 3
#define APP_TASK_FLAG_PRIO 2
#define APP_TASK_USART1_CMD_PRIO 2

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define APP_TASK_START_STK_SIZE 128
#define APP_TASK_START_STK_SIZE 128

#define APP_TASK_LED2_STK_SIZE 512
#define APP_TASK_LED3_STK_SIZE 512
#define APP_TASK_KEY_STK_SIZE 512
#define APP_TASK_FLAG_STK_SIZE 512
#define APP_TASK_USART1_CMD_STK_SIZE 1024

/*
*********************************************************************************************************
*                                    BSP CONFIGURATION: RS-232
*********************************************************************************************************
*/

#define BSP_CFG_SER_COMM_SEL BSP_SER_COMM_UART_02
#define BSP_CFG_TS_TMR_SEL 2

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/
#if 0
#define TRACE_LEVEL_OFF 0
#define TRACE_LEVEL_INFO 1
#define TRACE_LEVEL_DEBUG 2
#endif

#define APP_TRACE_LEVEL TRACE_LEVEL_INFO
#define APP_TRACE BSP_Ser_Printf

#define APP_TRACE_INFO(x) ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(APP_TRACE x) : (void)0)
#define APP_TRACE_DEBUG(x) ((APP_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(APP_TRACE x) : (void)0)

#endif

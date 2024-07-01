/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include <string.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

// OS_MEM  mem;                    //声明内存管理对象
// uint8_t ucArray [ 3 ] [ 20 ];   //声明内存分区大小

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static OS_TCB AppTaskStartTCB; // 任务控制块

static OS_TCB AppTaskLed2TCB;
static OS_TCB AppTaskLed3TCB;
static OS_TCB AppTaskLed3TCB;
static OS_TCB AppTaskKeyTCB;
static OS_TCB AppTaskFlagTCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE]; // 任务堆栈

static CPU_STK AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE];
static CPU_STK AppTaskLed3Stk[APP_TASK_LED3_STK_SIZE];

static CPU_STK AppTaskKeyStk[APP_TASK_KEY_STK_SIZE];
static CPU_STK AppTaskFlagStk[APP_TASK_FLAG_STK_SIZE];

/*
*********************************************************************************************************
*                                         Variables
*********************************************************************************************************
*/
uint32_t TimeStart; /* 定义三个全局变量 */
uint32_t TimeEnd;
uint32_t TimeUse;

OS_FLAG_GRP flag_grp;		   // 声明事件标志组
#define KEY0_EVENT (0x01 << 0) // 设置事件掩码的位0
#define KEY1_EVENT (0x01 << 1) // 设置事件掩码的位1

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void AppTaskStart(void *p_arg); // 任务函数声明

static void AppTaskLed2(void *p_arg);
static void AppTaskLed3(void *p_arg);
static void AppTaskKey(void *p_arg);
static void AppTaskPend(void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
	OS_ERR err;

	OSInit(&err); // 初始化 uC/OS-III

	/* 创建起始任务 */
	OSTaskCreate((OS_TCB *)&AppTaskStartTCB,						  // 任务控制块地址
				 (CPU_CHAR *)"App Task Start",						  // 任务名称
				 (OS_TASK_PTR)AppTaskStart,							  // 任务函数
				 (void *)0,											  // 传递给任务函数（形参p_arg）的实参
				 (OS_PRIO)APP_TASK_START_PRIO,						  // 任务的优先级
				 (CPU_STK *)&AppTaskStartStk[0],					  // 任务堆栈的基地址
				 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,		  // 任务堆栈空间剩下1/10时限制其增长
				 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,				  // 任务堆栈空间（单位：sizeof(CPU_STK)）
				 (OS_MSG_QTY)5u,									  // 任务可接收的最大消息数
				 (OS_TICK)0u,										  // 任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
				 (void *)0,											  // 任务扩展（0表不扩展）
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // 任务选项
				 (OS_ERR *)&err);									  // 返回错误类型

	OSStart(&err); // 启动多任务管理（交由uC/OS-III控制）
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

#define HSE_VALUE ((uint32_t)8000000) // 假设 HSE 是 8 MHz

static void AppTaskStart(void *p_arg)
{
	CPU_INT32U cpu_clk_freq;
	CPU_INT32U cnts;
	OS_ERR err;

	(void)p_arg;

	BSP_Init(); // 板级初始化
	CPU_Init(); // 初始化 CPU 组件（时间戳、关中断时间测量和主机名）

	cpu_clk_freq = BSP_CPU_ClkFreq();					 // 获取 CPU 内核时钟频率（SysTick 工作时钟）
	cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz; // 根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
	OS_CPU_SysTickInit(cnts);							 // 调用 SysTick 初始化函数，设置定时器计数值和启动定时器

	Mem_Init(); // 初始化内存管理组件（堆内存池和内存池表）

#if OS_CFG_STAT_TASK_EN > 0u	  // 如果使能（默认使能）了统计任务
	OSStatTaskCPUUsageInit(&err); // 计算没有应用任务（只有空闲任务）运行时 CPU 的（最大）
#endif							  // 容量（决定 OS_Stat_IdleCtrMax 的值，为后面计算 CPU
								  // 使用率使用）。
	CPU_IntDisMeasMaxCurReset();  // 复位（清零）当前最大关中断时间

	/* 创建事件标志组 flag_grp */
	OSFlagCreate((OS_FLAG_GRP *)&flag_grp,	  // 指向事件标志组的指针
				 (CPU_CHAR *)"FLAG For Test", // 事件标志组的名字
				 (OS_FLAGS)0,				  // 事件标志组的初始值
				 (OS_ERR *)&err);			  // 返回错误类型

	/* 配置时间片轮转调度 */
	OSSchedRoundRobinCfg((CPU_BOOLEAN)DEF_ENABLED, // 使能时间片轮转调度
						 (OS_TICK)0,			   // 把 OSCfg_TickRate_Hz / 10 设为默认时间片值
						 (OS_ERR *)&err);		   // 返回错误类型

	printf("CPU内核时钟频率：%d,SysTick定时器的计数值%d\n", cpu_clk_freq, cnts);

	/* 创建 LED2 任务 */
	OSTaskCreate((OS_TCB *)&AppTaskLed2TCB,							  // 任务控制块地址
				 (CPU_CHAR *)"App Task Led2",						  // 任务名称
				 (OS_TASK_PTR)AppTaskLed2,							  // 任务函数
				 (void *)0,											  // 传递给任务函数（形参p_arg）的实参
				 (OS_PRIO)APP_TASK_LED2_PRIO,						  // 任务的优先级
				 (CPU_STK *)&AppTaskLed2Stk[0],						  // 任务堆栈的基地址
				 (CPU_STK_SIZE)APP_TASK_LED2_STK_SIZE / 10,			  // 任务堆栈空间剩下1/10时限制其增长
				 (CPU_STK_SIZE)APP_TASK_LED2_STK_SIZE,				  // 任务堆栈空间（单位：sizeof(CPU_STK)）
				 (OS_MSG_QTY)5u,									  // 任务可接收的最大消息数
				 (OS_TICK)0u,										  // 任务的时间片节拍数（0表默认值）
				 (void *)0,											  // 任务扩展（0表不扩展）
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // 任务选项
				 (OS_ERR *)&err);									  // 返回错误类型

	/* 创建 LED3 任务 */
	OSTaskCreate((OS_TCB *)&AppTaskLed3TCB,							  // 任务控制块地址
				 (CPU_CHAR *)"App Task Led3",						  // 任务名称
				 (OS_TASK_PTR)AppTaskLed3,							  // 任务函数
				 (void *)0,											  // 传递给任务函数（形参p_arg）的实参
				 (OS_PRIO)APP_TASK_LED3_PRIO,						  // 任务的优先级
				 (CPU_STK *)&AppTaskLed3Stk[0],						  // 任务堆栈的基地址
				 (CPU_STK_SIZE)APP_TASK_LED3_STK_SIZE / 10,			  // 任务堆栈空间剩下1/10时限制其增长
				 (CPU_STK_SIZE)APP_TASK_LED3_STK_SIZE,				  // 任务堆栈空间（单位：sizeof(CPU_STK)）
				 (OS_MSG_QTY)5u,									  // 任务可接收的最大消息数
				 (OS_TICK)0u,										  // 任务的时间片节拍数（0表默认值）
				 (void *)0,											  // 任务扩展（0表不扩展）
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // 任务选项
				 (OS_ERR *)&err);									  // 返回错误类型

	/* 创建 KEY 任务 */
	OSTaskCreate((OS_TCB *)&AppTaskKeyTCB,							  // 任务控制块地址
				 (CPU_CHAR *)"App Task Key",						  // 任务名称
				 (OS_TASK_PTR)AppTaskKey,							  // 任务函数
				 (void *)0,											  // 传递给任务函数（形参p_arg）的实参
				 (OS_PRIO)APP_TASK_KEY_PRIO,						  // 任务的优先级
				 (CPU_STK *)&AppTaskKeyStk[0],						  // 任务堆栈的基地址
				 (CPU_STK_SIZE)APP_TASK_KEY_STK_SIZE / 10,			  // 任务堆栈空间剩下1/10时限制其增长
				 (CPU_STK_SIZE)APP_TASK_KEY_STK_SIZE,				  // 任务堆栈空间（单位：sizeof(CPU_STK)）
				 (OS_MSG_QTY)5u,									  // 任务可接收的最大消息数
				 (OS_TICK)0u,										  // 任务的时间片节拍数（0表默认值）
				 (void *)0,											  // 任务扩展（0表不扩展）
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // 任务选项
				 (OS_ERR *)&err);									  // 返回错误类型

	/* 创建 FLAG PEND 任务 */
	OSTaskCreate((OS_TCB *)&AppTaskFlagTCB,							  // 任务控制块地址
				 (CPU_CHAR *)"App Task pend",						  // 任务名称
				 (OS_TASK_PTR)AppTaskPend,							  // 任务函数
				 (void *)0,											  // 传递给任务函数（形参p_arg）的实参
				 (OS_PRIO)APP_TASK_FLAG_PRIO,						  // 任务的优先级
				 (CPU_STK *)&AppTaskFlagStk[0],						  // 任务堆栈的基地址
				 (CPU_STK_SIZE)APP_TASK_FLAG_STK_SIZE / 10,			  // 任务堆栈空间剩下1/10时限制其增长
				 (CPU_STK_SIZE)APP_TASK_FLAG_STK_SIZE,				  // 任务堆栈空间（单位：sizeof(CPU_STK)）
				 (OS_MSG_QTY)5u,									  // 任务可接收的最大消息数
				 (OS_TICK)0u,										  // 任务的时间片节拍数（0表默认值）
				 (void *)0,											  // 任务扩展（0表不扩展）
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // 任务选项
				 (OS_ERR *)&err);									  // 返回错误类型

	OSTaskDel(0, &err); // 删除起始任务本身，该任务不再运行
}

/*
*********************************************************************************************************
*                                          LED1 TASK
*********************************************************************************************************
*/

static void AppTaskLed2(void *p_arg)
{
	OS_ERR err;
	OS_REG value;

	(void)p_arg;

	while (DEF_TRUE)
	{				 // 任务体，通常写成一个死循环
		LED2_TOGGLE; // 切换 LED1 的亮灭状态

		value = OSTaskRegGet(0, 0, &err); // 获取自身任务寄存器值

		if (value < 10) // 如果任务寄存器值<10
		{
			OSTaskRegSet(0, 0, ++value, &err); // 继续累加任务寄存器值
		}
		else // 如果累加到10
		{
			OSTaskRegSet(0, 0, 1, &err); // 将任务寄存器值归0

			OSTaskResume(&AppTaskLed3TCB, &err); // 恢复 LED3 任务
			// printf("恢复LED2任务！\n");
			printf("resume task led3\n");
		}
		TimeStart = OS_TS_GET();
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err); // 相对性延时1000个时钟节拍（1s）
		TimeEnd = OS_TS_GET();
		TimeUse = TimeEnd - TimeStart;
		// printf("start:%lu,end:%lu,total:%lu\n", TimeStart, TimeEnd, TimeUse);
		// 将 TimeUse 从时钟频率计数转换为秒
		float timeUseInSeconds = TimeUse / (float)BSP_CPU_ClkFreq();
		// printf("total time in seconds: %.2f\n", timeUseInSeconds);
	}
}

/*
*********************************************************************************************************
*                                          LED2 TASK
*********************************************************************************************************
*/

static void AppTaskLed3(void *p_arg)
{
	OS_ERR err;
	OS_REG value;

	(void)p_arg;

	while (DEF_TRUE)
	{									  // 任务体，通常写成一个死循环
		LED3_TOGGLE;					  // 切换 LED2 的亮灭状态
		value = OSTaskRegGet(0, 0, &err); // 获取自身任务寄存器值

		if (value < 5) // 如果任务寄存器值<5
		{
			OSTaskRegSet(0, 0, ++value, &err); // 继续累加任务寄存器值
		}
		else // 如果累加到5
		{
			OSTaskRegSet(0, 0, 0, &err); // 将任务寄存器值归0

			printf("suspend task led3(self)\n");
			OSTaskSuspend(0, &err); // 挂起自身
		}

		OSTimeDly(1000, OS_OPT_TIME_DLY, &err); // 相对性延时1000个时钟节拍（1s）
	}
}

static void AppTaskKey(void *p_arg)
{
	OS_ERR err;
	static u8 k0 = 0;

	(void)p_arg;
	printf("ERR:%d\n", err);
	while (DEF_TRUE)
	{
		if (Key_Scan(KEY0_GPIO_PORT, KEY0_PIN) == KEY_ON)
		{
			printf("KEY0 was pressed\n,k0=%d,~K0=%d", k0, ~k0);
			OSFlagPost((OS_FLAG_GRP *)&flag_grp, // 将标志组的BIT0置1
					   (OS_FLAGS)KEY0_EVENT,
					   (OS_OPT)OS_OPT_POST_FLAG_SET,
					   (OS_ERR *)&err);
			if (k0 == 0)
			{
				OSTaskSuspend((OS_TCB *)&AppTaskLed2TCB, &err);
				k0 = 1;
			}
			else
			{
				OSTaskResume((OS_TCB *)&AppTaskLed2TCB, &err);
				k0 = 0;
			}
		}
		if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
		{
			OSFlagPost((OS_FLAG_GRP *)&flag_grp, // 将标志组的BIT1置1
					   (OS_FLAGS)KEY1_EVENT,
					   (OS_OPT)OS_OPT_POST_FLAG_SET,
					   (OS_ERR *)&err);
			printf("KEY1 was pressed, LED3 was suspend,err:%d\n", err);
			OSTaskSuspend((OS_TCB *)&AppTaskLed3TCB, &err);
		}
		OSTimeDlyHMSM(0, 0, 0, 20, OS_OPT_TIME_DLY, &err); // 每20ms扫描一次
	}
}

static void AppTaskPend(void *p_arg)
{
	OS_ERR err;
	OS_FLAGS flags_rdy;
	(void)p_arg;
	while (DEF_TRUE)
	{
		// 等待标志组的的BIT0和BIT1均被置1
		flags_rdy = OSFlagPend((OS_FLAG_GRP *)&flag_grp,
							   (OS_FLAGS)(KEY0_EVENT | KEY1_EVENT),
							   (OS_TICK)0, (OS_OPT)OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_BLOCKING | OS_OPT_PEND_FLAG_CONSUME,
							   (CPU_TS *)0,
							   (OS_ERR *)&err);
		if ((flags_rdy & (KEY0_EVENT | KEY1_EVENT)) == (KEY0_EVENT | KEY1_EVENT))
		{
			printf("K0 and K1 was pressed!\n");
		}
	}
}

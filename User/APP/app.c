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

// OS_MEM  mem;                    //�����ڴ�������
// uint8_t ucArray [ 3 ] [ 20 ];   //�����ڴ������С

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static OS_TCB AppTaskStartTCB; // ������ƿ�

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

static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE]; // �����ջ

static CPU_STK AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE];
static CPU_STK AppTaskLed3Stk[APP_TASK_LED3_STK_SIZE];

static CPU_STK AppTaskKeyStk[APP_TASK_KEY_STK_SIZE];
static CPU_STK AppTaskFlagStk[APP_TASK_FLAG_STK_SIZE];

/*
*********************************************************************************************************
*                                         Variables
*********************************************************************************************************
*/
uint32_t TimeStart; /* ��������ȫ�ֱ��� */
uint32_t TimeEnd;
uint32_t TimeUse;

OS_FLAG_GRP flag_grp;		   // �����¼���־��
#define KEY0_EVENT (0x01 << 0) // �����¼������λ0
#define KEY1_EVENT (0x01 << 1) // �����¼������λ1

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void AppTaskStart(void *p_arg); // ����������

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

	OSInit(&err); // ��ʼ�� uC/OS-III

	/* ������ʼ���� */
	OSTaskCreate((OS_TCB *)&AppTaskStartTCB,						  // ������ƿ��ַ
				 (CPU_CHAR *)"App Task Start",						  // ��������
				 (OS_TASK_PTR)AppTaskStart,							  // ������
				 (void *)0,											  // ���ݸ����������β�p_arg����ʵ��
				 (OS_PRIO)APP_TASK_START_PRIO,						  // ��������ȼ�
				 (CPU_STK *)&AppTaskStartStk[0],					  // �����ջ�Ļ���ַ
				 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,		  // �����ջ�ռ�ʣ��1/10ʱ����������
				 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,				  // �����ջ�ռ䣨��λ��sizeof(CPU_STK)��
				 (OS_MSG_QTY)5u,									  // ����ɽ��յ������Ϣ��
				 (OS_TICK)0u,										  // �����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
				 (void *)0,											  // ������չ��0����չ��
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // ����ѡ��
				 (OS_ERR *)&err);									  // ���ش�������

	OSStart(&err); // �����������������uC/OS-III���ƣ�
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

#define HSE_VALUE ((uint32_t)8000000) // ���� HSE �� 8 MHz

static void AppTaskStart(void *p_arg)
{
	CPU_INT32U cpu_clk_freq;
	CPU_INT32U cnts;
	OS_ERR err;

	(void)p_arg;

	BSP_Init(); // �弶��ʼ��
	CPU_Init(); // ��ʼ�� CPU �����ʱ��������ж�ʱ���������������

	cpu_clk_freq = BSP_CPU_ClkFreq();					 // ��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
	cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz; // �����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
	OS_CPU_SysTickInit(cnts);							 // ���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��

	Mem_Init(); // ��ʼ���ڴ������������ڴ�غ��ڴ�ر�

#if OS_CFG_STAT_TASK_EN > 0u	  // ���ʹ�ܣ�Ĭ��ʹ�ܣ���ͳ������
	OSStatTaskCPUUsageInit(&err); // ����û��Ӧ������ֻ�п�����������ʱ CPU �ģ����
#endif							  // ���������� OS_Stat_IdleCtrMax ��ֵ��Ϊ������� CPU
								  // ʹ����ʹ�ã���
	CPU_IntDisMeasMaxCurReset();  // ��λ�����㣩��ǰ�����ж�ʱ��

	/* �����¼���־�� flag_grp */
	OSFlagCreate((OS_FLAG_GRP *)&flag_grp,	  // ָ���¼���־���ָ��
				 (CPU_CHAR *)"FLAG For Test", // �¼���־�������
				 (OS_FLAGS)0,				  // �¼���־��ĳ�ʼֵ
				 (OS_ERR *)&err);			  // ���ش�������

	/* ����ʱ��Ƭ��ת���� */
	OSSchedRoundRobinCfg((CPU_BOOLEAN)DEF_ENABLED, // ʹ��ʱ��Ƭ��ת����
						 (OS_TICK)0,			   // �� OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
						 (OS_ERR *)&err);		   // ���ش�������

	printf("CPU�ں�ʱ��Ƶ�ʣ�%d,SysTick��ʱ���ļ���ֵ%d\n", cpu_clk_freq, cnts);

	/* ���� LED2 ���� */
	OSTaskCreate((OS_TCB *)&AppTaskLed2TCB,							  // ������ƿ��ַ
				 (CPU_CHAR *)"App Task Led2",						  // ��������
				 (OS_TASK_PTR)AppTaskLed2,							  // ������
				 (void *)0,											  // ���ݸ����������β�p_arg����ʵ��
				 (OS_PRIO)APP_TASK_LED2_PRIO,						  // ��������ȼ�
				 (CPU_STK *)&AppTaskLed2Stk[0],						  // �����ջ�Ļ���ַ
				 (CPU_STK_SIZE)APP_TASK_LED2_STK_SIZE / 10,			  // �����ջ�ռ�ʣ��1/10ʱ����������
				 (CPU_STK_SIZE)APP_TASK_LED2_STK_SIZE,				  // �����ջ�ռ䣨��λ��sizeof(CPU_STK)��
				 (OS_MSG_QTY)5u,									  // ����ɽ��յ������Ϣ��
				 (OS_TICK)0u,										  // �����ʱ��Ƭ��������0��Ĭ��ֵ��
				 (void *)0,											  // ������չ��0����չ��
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // ����ѡ��
				 (OS_ERR *)&err);									  // ���ش�������

	/* ���� LED3 ���� */
	OSTaskCreate((OS_TCB *)&AppTaskLed3TCB,							  // ������ƿ��ַ
				 (CPU_CHAR *)"App Task Led3",						  // ��������
				 (OS_TASK_PTR)AppTaskLed3,							  // ������
				 (void *)0,											  // ���ݸ����������β�p_arg����ʵ��
				 (OS_PRIO)APP_TASK_LED3_PRIO,						  // ��������ȼ�
				 (CPU_STK *)&AppTaskLed3Stk[0],						  // �����ջ�Ļ���ַ
				 (CPU_STK_SIZE)APP_TASK_LED3_STK_SIZE / 10,			  // �����ջ�ռ�ʣ��1/10ʱ����������
				 (CPU_STK_SIZE)APP_TASK_LED3_STK_SIZE,				  // �����ջ�ռ䣨��λ��sizeof(CPU_STK)��
				 (OS_MSG_QTY)5u,									  // ����ɽ��յ������Ϣ��
				 (OS_TICK)0u,										  // �����ʱ��Ƭ��������0��Ĭ��ֵ��
				 (void *)0,											  // ������չ��0����չ��
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // ����ѡ��
				 (OS_ERR *)&err);									  // ���ش�������

	/* ���� KEY ���� */
	OSTaskCreate((OS_TCB *)&AppTaskKeyTCB,							  // ������ƿ��ַ
				 (CPU_CHAR *)"App Task Key",						  // ��������
				 (OS_TASK_PTR)AppTaskKey,							  // ������
				 (void *)0,											  // ���ݸ����������β�p_arg����ʵ��
				 (OS_PRIO)APP_TASK_KEY_PRIO,						  // ��������ȼ�
				 (CPU_STK *)&AppTaskKeyStk[0],						  // �����ջ�Ļ���ַ
				 (CPU_STK_SIZE)APP_TASK_KEY_STK_SIZE / 10,			  // �����ջ�ռ�ʣ��1/10ʱ����������
				 (CPU_STK_SIZE)APP_TASK_KEY_STK_SIZE,				  // �����ջ�ռ䣨��λ��sizeof(CPU_STK)��
				 (OS_MSG_QTY)5u,									  // ����ɽ��յ������Ϣ��
				 (OS_TICK)0u,										  // �����ʱ��Ƭ��������0��Ĭ��ֵ��
				 (void *)0,											  // ������չ��0����չ��
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // ����ѡ��
				 (OS_ERR *)&err);									  // ���ش�������

	/* ���� FLAG PEND ���� */
	OSTaskCreate((OS_TCB *)&AppTaskFlagTCB,							  // ������ƿ��ַ
				 (CPU_CHAR *)"App Task pend",						  // ��������
				 (OS_TASK_PTR)AppTaskPend,							  // ������
				 (void *)0,											  // ���ݸ����������β�p_arg����ʵ��
				 (OS_PRIO)APP_TASK_FLAG_PRIO,						  // ��������ȼ�
				 (CPU_STK *)&AppTaskFlagStk[0],						  // �����ջ�Ļ���ַ
				 (CPU_STK_SIZE)APP_TASK_FLAG_STK_SIZE / 10,			  // �����ջ�ռ�ʣ��1/10ʱ����������
				 (CPU_STK_SIZE)APP_TASK_FLAG_STK_SIZE,				  // �����ջ�ռ䣨��λ��sizeof(CPU_STK)��
				 (OS_MSG_QTY)5u,									  // ����ɽ��յ������Ϣ��
				 (OS_TICK)0u,										  // �����ʱ��Ƭ��������0��Ĭ��ֵ��
				 (void *)0,											  // ������չ��0����չ��
				 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // ����ѡ��
				 (OS_ERR *)&err);									  // ���ش�������

	OSTaskDel(0, &err); // ɾ����ʼ������������������
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
	{				 // �����壬ͨ��д��һ����ѭ��
		LED2_TOGGLE; // �л� LED1 ������״̬

		value = OSTaskRegGet(0, 0, &err); // ��ȡ��������Ĵ���ֵ

		if (value < 10) // �������Ĵ���ֵ<10
		{
			OSTaskRegSet(0, 0, ++value, &err); // �����ۼ�����Ĵ���ֵ
		}
		else // ����ۼӵ�10
		{
			OSTaskRegSet(0, 0, 1, &err); // ������Ĵ���ֵ��0

			OSTaskResume(&AppTaskLed3TCB, &err); // �ָ� LED3 ����
			// printf("�ָ�LED2����\n");
			printf("resume task led3\n");
		}
		TimeStart = OS_TS_GET();
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err); // �������ʱ1000��ʱ�ӽ��ģ�1s��
		TimeEnd = OS_TS_GET();
		TimeUse = TimeEnd - TimeStart;
		// printf("start:%lu,end:%lu,total:%lu\n", TimeStart, TimeEnd, TimeUse);
		// �� TimeUse ��ʱ��Ƶ�ʼ���ת��Ϊ��
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
	{									  // �����壬ͨ��д��һ����ѭ��
		LED3_TOGGLE;					  // �л� LED2 ������״̬
		value = OSTaskRegGet(0, 0, &err); // ��ȡ��������Ĵ���ֵ

		if (value < 5) // �������Ĵ���ֵ<5
		{
			OSTaskRegSet(0, 0, ++value, &err); // �����ۼ�����Ĵ���ֵ
		}
		else // ����ۼӵ�5
		{
			OSTaskRegSet(0, 0, 0, &err); // ������Ĵ���ֵ��0

			printf("suspend task led3(self)\n");
			OSTaskSuspend(0, &err); // ��������
		}

		OSTimeDly(1000, OS_OPT_TIME_DLY, &err); // �������ʱ1000��ʱ�ӽ��ģ�1s��
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
			OSFlagPost((OS_FLAG_GRP *)&flag_grp, // ����־���BIT0��1
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
			OSFlagPost((OS_FLAG_GRP *)&flag_grp, // ����־���BIT1��1
					   (OS_FLAGS)KEY1_EVENT,
					   (OS_OPT)OS_OPT_POST_FLAG_SET,
					   (OS_ERR *)&err);
			printf("KEY1 was pressed, LED3 was suspend,err:%d\n", err);
			OSTaskSuspend((OS_TCB *)&AppTaskLed3TCB, &err);
		}
		OSTimeDlyHMSM(0, 0, 0, 20, OS_OPT_TIME_DLY, &err); // ÿ20msɨ��һ��
	}
}

static void AppTaskPend(void *p_arg)
{
	OS_ERR err;
	OS_FLAGS flags_rdy;
	(void)p_arg;
	while (DEF_TRUE)
	{
		// �ȴ���־��ĵ�BIT0��BIT1������1
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

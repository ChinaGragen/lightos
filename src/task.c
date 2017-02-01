/*
 * task.c
 *
 *  Created on: 2017��1��25��
 *      Author: xzy47
 */


#include <stdlib.h>
#include "lightos.h"
#include "stm32f4xx_hal.h"
#include "task.h"

#define HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))

l_uint8_t Cur_TaskID = 0;
l_uint8_t l_nextTaskID = 0;
l_uint8_t l_taskNumber = 0;

l_uint32_t PSP_array[LCONFIG_TASK_MAX_NUMBER];

l_uint32_t LTaskIncrementTick(void)
{
	static l_uint8_t osTick;

	if(LCONFIG_OS_TIMESLICE == 0)
	{
		return 0;
	}

	if(++osTick == LCONFIG_OS_TIMESLICE)
	{
		 osTick = 0;
		 if(++l_nextTaskID >= 2)
			 l_nextTaskID = 0;
	}
}

l_uint32_t LTaskCreate(l_uint8_t          cTID,
                       LTaskFunction_t    pxEntry,
                       const char * const pcName,
                       const l_uint16_t   usStackDepth)
{
	l_uint32_t *task_stack = malloc(usStackDepth * sizeof(l_uint32_t));
	PSP_array[cTID] = ((unsigned int) task_stack) + (usStackDepth * sizeof(l_uint32_t)) - 16*4;
	//PSP_array�д洢��Ϊtask0_stack�����β��ַ-16*4����task0_stack[1023-16]��ַ
	HW32_REG((PSP_array[cTID] + (14<<2))) = (unsigned long) pxEntry; /* PC */
	//task0��PC�洢��task0_stack[1023-16]��ַ  +14<<2�У���task0_stack[1022]��
	HW32_REG((PSP_array[cTID] + (15<<2))) = 0x01000000;            /* xPSR */
}

void LTaskStartScheduler(void)
{
	LPORT_SYSTICK_ENABLE;
}

void LTaskStopScheduler(void)
{
	LPORT_SYSTICK_DISABLE;
}

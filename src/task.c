/*
 * task.c
 *
 *  Created on: 2017��1��25��
 *      Author: xzy47
 */


#include "lightos.h"
#include "stm32f4xx_hal.h"

#define HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))

l_uint8_t Cur_TaskID = 0;
l_uint8_t l_nextTaskID = 0;

l_uint32_t PSP_array[3];

l_uint32_t task0_stack[128];
l_uint32_t task1_stack[128];


extern void Task_0();	 //����0
extern void Task_1();	 //����1
extern void Task_2();	 //����2

l_uint32_t LTaskIncrementTick( void )
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

l_uint32_t LTaskCreate( void )
{
	l_uint32_t tmp = sizeof task0_stack;
	PSP_array[0] = ((unsigned int) task0_stack) + (sizeof task0_stack) - 16*4;
	    //PSP_array�д洢��Ϊtask0_stack�����β��ַ-16*4����task0_stack[1023-16]��ַ
	    HW32_REG((PSP_array[0] + (14<<2))) = (unsigned long) Task_0; /* PC */
	    //task0��PC�洢��task0_stack[1023-16]��ַ  +14<<2�У���task0_stack[1022]��
	    HW32_REG((PSP_array[0] + (15<<2))) = 0x01000000;            /* xPSR */
	   // HW32_REG((PSP_array[0] + (13<<2))) = (unsigned long) Task_0;            /* LR */


	    PSP_array[1] = ((unsigned int) task1_stack) + (sizeof task1_stack) - 16*4;
	    HW32_REG((PSP_array[1] + (14<<2))) = (unsigned long) Task_1; /* PC */
	    HW32_REG((PSP_array[1] + (15<<2))) = 0x01000000;            /* xPSR */
	   // HW32_REG((PSP_array[1] + (13<<2))) = (unsigned long) Task_1;            /* LR */

	Cur_TaskID = 0;



}

void LTaskStartScheduler( void )
{
	/* ����PSPָ������0��ջ��ջ�� */
	__asm volatile ("MSR psp, %0\n" : : "r" ((PSP_array[Cur_TaskID] + 16*4)) :"sp" );

	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	__asm volatile ("MSR control, %0" : : "r" (0x3) : "memory");

	__asm volatile ("isb 0xF":::"memory");

		    //Task_0();
	__asm volatile ("b Task_0");

}

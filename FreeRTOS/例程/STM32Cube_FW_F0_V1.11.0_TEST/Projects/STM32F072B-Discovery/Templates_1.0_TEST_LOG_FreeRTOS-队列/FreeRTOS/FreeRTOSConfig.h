/*
 * FreeRTOS V202411.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Prevent C code being included by the IAR assembler. */
#ifndef __IASMARM__
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif

#define configUSE_PREEMPTION			1		/* 1=抢占式调度, 0=协作式调度 */
#define configUSE_IDLE_HOOK				0		/* 1=启用空闲任务钩子函数, 0=禁用 */
#define configUSE_TICK_HOOK				1		/* 1=启用Tick中断钩子函数, 0=禁用 */
#define configCPU_CLOCK_HZ				( SystemCoreClock )	/* CPU主频(Hz), 用于SysTick配置 */
#define configTICK_RATE_HZ				( ( TickType_t ) 500 )	/* Tick频率(Hz), 即每秒Tick次数, 500=2ms一次 */
#define configMAX_PRIORITIES			( 5 )	/* 任务最大优先级数量, 0为最低优先级 */
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 60 )	/* 最小任务栈大小(单位:字, 即60*4=240字节) */
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 9216 ) )	/* FreeRTOS堆内存总大小(字节)  测试动态分配8192字节  静态分配4096字节*/
#define configMAX_TASK_NAME_LEN			( 9 )	/* 任务名称最大长度(含'\0') */
#define configUSE_TRACE_FACILITY		1		/* 1=启用跟踪功能, 含队列/信号量等调试信息 */
#define configUSE_16_BIT_TICKS			0		/* 0=TickType_t为32位, 1=16位(节省内存但溢出快) */
#define configIDLE_SHOULD_YIELD			1		/* 1=空闲任务主动让出CPU给同优先级任务 */
#define configUSE_MUTEXES				1		/* 1=启用互斥量(含优先级继承机制) */
#define configQUEUE_REGISTRY_SIZE		8		/* 队列注册表大小, 用于内核感知调试器 */
#define configCHECK_FOR_STACK_OVERFLOW	2		/* 0=禁用, 1=方法一(仅检查指针), 2=方法二(检查栈填充模式,更可靠) */
#define configUSE_RECURSIVE_MUTEXES		1		/* 1=启用递归互斥量(同一任务可多次获取) */
#define configUSE_MALLOC_FAILED_HOOK	1		/* 1=启用内存分配失败钩子函数 */
#define configUSE_APPLICATION_TASK_TAG	0		/* 1=启用任务标签(可关联自定义数据), 0=禁用 */
#define configUSE_COUNTING_SEMAPHORES	1		/* 1=启用计数信号量 */
#define configGENERATE_RUN_TIME_STATS	0		/* 1=启用运行时间统计(需提供高精度定时器), 0=禁用 */
#define configUSE_QUEUE_SETS			1		/* 1=启用队列集(同时监听多个队列/信号量) */
#define configENABLE_MPU				0		/* 0=禁用MPU(Cortex-M0无MPU), 1=启用MPU */

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 			0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS				0
#define configTIMER_TASK_PRIORITY		( 2 )
#define configTIMER_QUEUE_LENGTH		5
#define configTIMER_TASK_STACK_DEPTH	( 128 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	1
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1
#define INCLUDE_eTaskGetState			1

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

/* Disable the runtime check on the installation of the FreeRTOS handlers. The
port code that performs the check expects the handlers to be installed directly
into the vector table. However, XMC1000 has a ROM-based vector table, so it does
not contain the OS handler vectors directly. */
#define configCHECK_HANDLER_INSTALLATION  0

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names - or at least those used in the unmodified vector table. */
#define xPortPendSVHandler PendSV_Handler
#define vPortSVCHandler SVC_Handler
#define xPortSysTickHandler SysTick_Handler

#define INCLUDE_xTaskGetSchedulerState  1

#define configSUPPORT_STATIC_ALLOCATION 0
#define configUSE_STATS_FORMATTING_FUNCTIONS 0

/* Bump up the priority of recmuCONTROLLING_TASK_PRIORITY to prevent false
positive errors being reported considering the priority of other tasks in the
system. */
#define recmuCONTROLLING_TASK_PRIORITY ( configMAX_PRIORITIES - 1 )

#endif /* FREERTOS_CONFIG_H */
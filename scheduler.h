/*
 * =======================================================
 * = A lightweight Asynchronous Real Time Task Scheduler =
 * = based/compatible with FreeRTOS API                  =
 * = Written by St4rk                                    =
 * = FreeRTOS WebSite: http://www.freertos.org/          =
 * =======================================================
 */


#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "schedulerConfig.h"


/**
 * Source: FreeRTOS
 * This MACRO is used to store the 
 * current task context in stack
 */
#define portRESTORE_CONTEXT() \
  asm volatile ( \
    "lds r26, c_Task \n\t" \
    "lds r27, c_Task + 1 \n\t" \
    "ld r28, x+ \n\t" \
    "out __SP_L__, r28 \n\t" \
    "ld r29, x+ \n\t" \
    "out __SP_H__, r29 \n\t" \
    "pop r31 \n\t" \
    "pop r30 \n\t" \
    "pop r29 \n\t" \
    "pop r28 \n\t" \
    "pop r27 \n\t" \
    "pop r26 \n\t" \
    "pop r25 \n\t" \
    "pop r24 \n\t" \
    "pop r23 \n\t" \
    "pop r22 \n\t" \
    "pop r21 \n\t" \
    "pop r20 \n\t" \
    "pop r19 \n\t" \
    "pop r18 \n\t" \
    "pop r17 \n\t" \
    "pop r16 \n\t" \
    "pop r15 \n\t" \
    "pop r14 \n\t" \
    "pop r13 \n\t" \
    "pop r12 \n\t" \
    "pop r11 \n\t" \
    "pop r10 \n\t" \
    "pop r9 \n\t" \
    "pop r8 \n\t" \
    "pop r7 \n\t" \
    "pop r6 \n\t" \
    "pop r5 \n\t" \
    "pop r4 \n\t" \
    "pop r3 \n\t" \
    "pop r2 \n\t" \
    "pop r1 \n\t" \
    "pop r0 \n\t" \
    "out __SREG__, r0 \n\t" \
    "pop r0 \n\t" \
  )

/**
 * Source: FreeRTOS
 * This macro will restore the next
 * context from stack
 */
#define portSAVE_CONTEXT()           \
  asm volatile ( \
    "push r0 \n\t" \
    "in r0, __SREG__ \n\t" \
    "cli \n\t" \
    "push r0 \n\t" \
    "push r1 \n\t" \
    "clr r1 \n\t" \
    "push r2 \n\t" \
    "push r3 \n\t" \
    "push r4 \n\t" \
    "push r5 \n\t" \
    "push r6 \n\t" \
    "push r7 \n\t" \
    "push r8 \n\t" \
    "push r9 \n\t" \
    "push r10 \n\t" \
    "push r11 \n\t" \
    "push r12 \n\t" \
    "push r13 \n\t" \
    "push r14 \n\t" \
    "push r15 \n\t" \
    "push r16 \n\t" \
    "push r17 \n\t" \
    "push r18 \n\t" \
    "push r19 \n\t" \
    "push r20 \n\t" \
    "push r21 \n\t" \
    "push r22 \n\t" \
    "push r23 \n\t" \
    "push r24 \n\t" \
    "push r25 \n\t" \
    "push r26 \n\t" \
    "push r27 \n\t" \
    "push r28 \n\t" \
    "push r29 \n\t" \
    "push r30 \n\t" \
    "push r31 \n\t" \
    "lds r26, c_Task \n\t" \
    "lds r27, c_Task +1 \n\t" \
    "in r0, __SP_L__ \n\t" \
    "st x+, r0  \n\t" \
    "in r0, __SP_H__ \n\t" \
    "st x+, r0 \n\t" \
  )
/**
 * TaskHandle_t is used to hold the task id
 */
typedef unsigned char TaskHandle_t;
/**
 * UBaseType_t and BaseType_t are the most efficient, natural, type for the architecture
 * in our case (ATmega328p) is 8 bits(1 byte)
 */
typedef unsigned char UBaseType_t;
typedef char BaseType_t;

/**
 * Function pointer with void as argument
 */
typedef void (*TaskFunction_t)(void*);

/**
 * Scheduler tasks that will be created in xTaskCreate
 */
typedef struct task_t {
    UBaseType_t* SP_ctx;
	char name[configMAX_TASK_NAME_LEN];
	UBaseType_t priority;
	UBaseType_t state; 
	unsigned short tmr;
} task_t;


/**
 * Double Linked List used to create the Task List
 */
typedef struct node_t {
	struct node_t *next;
	task_t task;
	UBaseType_t id;
} node_t;



/**
 * ===========================================
 * This function is used to create a task
 * arguments
 * @ 1 - Task function entry-point
 * @ 2 - Task name
 * @ 3 - Stack Size
 * @ 4 - Parameters
 * @ 5 - Task Priority
 * @ 6 - Pass out the created task's handle
 * ===========================================
 */

#ifdef __cplusplus
extern "C" {
#endif
	BaseType_t xTaskCreate(  TaskFunction_t pvTaskCode,
									const char * const pcName,
									unsigned short usStackDepth,
									void *pvParameters,
									UBaseType_t uxPriority,
									TaskHandle_t *pxCreatedTask
								);
#ifdef __cplusplus
}
#endif

/**
 * =========================================================
 * This function is used to initialize the task scheduler
 * No arguments
 * =========================================================
 */
#ifdef __cplusplus
extern "C" {
#endif
	void xInitTaskScheduler();
#ifdef __cplusplus
}
#endif
/**
 * ==========================================================
 * This function is used to remove a task from the task list
 * arguments
 * @ 1 - Task ID
 * ==========================================================
 */
#ifdef __cplusplus
extern "C" {
#endif
	void vTaskDelete( TaskHandle_t xTask );
#ifdef __cplusplus
}
#endif

/**
 * ===========================================================
 * This function is used to sleep/delay the task for "n" ms.
 * The task is keep in SUSPENDED state 
 * @ 1 - Time in milliseconds
 * ===========================================================
 */
#ifdef __cplusplus
extern "C" {
#endif
	void vTaskDelay(const unsigned short ms);
#ifdef __cplusplus
}
#endif

#endif

/*
 * =======================================================
 * = A lightweight Asynchronous Real Time Task Scheduler =
 * = based/compatible with FreeRTOS API                  =
 * = Written by St4rk                                    =
 * = FreeRTOS WebSite: http://www.freertos.org/          =
 * =======================================================
 */


#ifndef _SCHEDULER_
#define _SCHEDULER_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "schedulerConfig.h"

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
typedef void (*fptr)(void*) TaskFunction_t;

/**
 * Scheduler tasks that will be created in xTaskCreate
 */
typedef struct task_t {
	char name[configMAX_TASK_NAME_LEN];
	TaskFunction_t entryPoint;
	UBaseType_t priority;
	UBaseType_t state;
} task_t;


/**
 * Double Linked List used to create the Task List
 */
typedef struct node_t {
	node_t *next;
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
extern BaseType_t xTaskCreate(  TaskFunction_t pvTaskCode,
								const char * const pcName,
								unsigned short usStackDepth,
								void *pvParameters,
								UBaseType_t uxPriority,
								TaskHandle_t *pxCreatedTask
							);


/**
 * =========================================================
 * This function is used to initialize the task scheduler
 * No arguments
 * Return:
 * @ 0 - OKAY
 * @ 1 - An error occured while initializing the scheduler
 * =========================================================
 */
extern BaseType_t xInitTaskScheduler();

/**
 * ==========================================================
 * This function is used to remove a task from the task list
 * arguments
 * @ 1 - Task ID
 * ===========================================================
 */
extern void vTaskDelete( TaskHandle_t xTask );

#endif
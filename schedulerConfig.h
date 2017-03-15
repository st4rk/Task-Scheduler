#ifndef _SCHEDULER_CONFIG_H_
#define _SCHEDULER_CONFIG_H_

/**
 * Max task size name, standard is 16, but as we are working with
 * an ATmega328p, I'm using 8 bytes
 */
#define configMAX_TASK_NAME_LEN 0x8

/**
 * The maximum stack size, I didn't find any information about maximum size in http://www.freertos.org/a00110.html
 * but as we are dealing with a very little SRAM (~2KBytes), I established a limit
 */
#define configMAX_TASK_STACK_SIZE 0x64

/**
 * As we have a very limited memory size, here will have the heap len
 */
#define configMAX_HEAP_LEN 0xFF


/**
 * The max number of task that the system can have
 * as we have limited memory, we define the max number here
 */
#define configMAX_SYS_TASK 0x4


/**
 * The lowest priority, the created task should use tskIDLE_PRIORITY+Priority
 * i.e: tskIDLE_PRIORITY+3
 */
#define tskIDLE_PRIORITY 0x0


/**
 * The Timer2 interrupt time, it's currently using 64 prescale + 16mhz to hae
 * 1 ms, if you want to change it, follow this:
 * desired_time = [(mcu_freq/prescale) * desired_time_in_second] - 1
 */

#define configTASK_SCHEDULER_TMR = 249;

/**
 * xTaskCreate Error List
 */

#define TASK_CREATE_INVALID_FUNCTION 0x1
#define TASK_CREATE_INVALID_STACK    0x2
#define TASK_CREATE_INVALID_TASK     0x3


/**
 * Task status
 */

#define TASK_STATUS_READY            0x0
#define TASK_STATUS_RUNNING          0x1
#define TASK_STATUS_BLOCKED          0x2
#define TASK_STATUS_SUSPENDED        0x3

#endif
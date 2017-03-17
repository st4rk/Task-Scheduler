#include "scheduler.h"


/**
 * Heap Memory Array used to heap allocation
 */
node_t __sysTask[configMAX_SYS_TASK];

/**
 * Global current task
 */
task_t * volatile c_Task = NULL;

/**
 * Task list head
 */
node_t * volatile taskList = NULL;

/**
 * Global Stack pointer from current task
 */
unsigned char* volatile stack_ptr = NULL;


/**
 * =============================================================
 * This function search for a free task in the __sysTask array
 * if no free task is found, it should return NULL
 * @ return either a pointer to the node or nullptr
 * =============================================================
 */
inline node_t* allocNode() {
	for (int i = 0; i < configMAX_SYS_TASK; i++) {
		if (__sysTask[i].id == 0x0) {
			return &__sysTask[i];
		} 
	}

	return NULL;
}

/**
 * ============================================
 * To remove the node, just clear its memory.
 * @ 1 - node pointer
 * ============================================
 */
inline void removeNode(node_t* node) {
	memset(node, 0x0, sizeof(node_t));
}

/**
 * ================================================================
 * This function is used to insert a new node into the linked list
 * @ 1 - node pointer
 * ================================================================
 */
inline void insertNewTask(node_t *task) {

	if (taskList == NULL) {
		taskList = task;
	} else {
		node_t *aux = taskList;

		while (1) {
			task->id++;
			
			if (aux->next == NULL) {
				aux->next = task;
				break;
			}

			aux = aux->next;
		}
	}
}

/**
 * ============================================================
 * This function is used to remove a node from the linked list
 * @ 1 - node pointer
 * ============================================================
 */
inline void removeTask( TaskHandle_t xTask ) {
	node_t *aux = taskList;

	while (1) {
		if (aux->id == xTask) {
			removeNode(aux);
			break;
		}

		if (aux->next == NULL) break;

		aux = aux->next;
	}
}


inline task_t* findHighPriorityTask() {
	node_t* aux = taskList;

	/**
	 * Check if the next is NULL, if yes, the root that has 
	 * the high priority
	 */

	while (1) {

		if (aux->task.state == TASK_STATUS_SUSPENDED) {

		//	while (1) {}
			/**
			 * Decrement the timer
			 */

		//	aux->task.tmr--;
			if (!aux->task.tmr) {
				aux->task.state = TASK_STATUS_READY;
			} else {
				if (aux->next != NULL) {
					aux = aux->next;
				} else {
					return NULL;
				}
			}
		}

		if (aux->next == NULL) break;

		if (aux->next->task.priority > aux->task.priority) {
			if (aux->next->task.state == TASK_STATUS_READY)
				aux = aux->next;
		}
	}


	return &aux->task;
}


/**
 * ===========================================================
 * This function is the timer2 compare interrupt handle
 * everytime that the timer reach the desired value
 * this interrupt handler will be triggered.
 * Here we will write our task scheduler that will
 * check which task is ready and that task has high-priority
 * and start it
 * ===========================================================
 */
ISR (TIMER2_COMPA_vect, ISR_NAKED) {

	/**
	 * Disable all interrupts, while we are in scheduler, no
	 * interrupts should happen
	 */
	cli();

	/**
	 * If it's running for the first time, you can't save the context
	 * because it's not the task, but the "loop" from arduino (probably)
	 */
	if (c_Task != NULL) {
		portSAVE_CONTEXT();

		if (c_Task->state == TASK_STATUS_RUNNING)
			c_Task->state = TASK_STATUS_READY;
	}

	
	/**
	 * Verify which task has the high priority
	 */
	c_Task = findHighPriorityTask();


	/**
	 * Global stack pointer from current task
	 */
	stack_ptr = c_Task->SP_ctx;

	/**
	 * Restore the current high priority task context
	 */
	portRESTORE_CONTEXT();

	/**
	 * AVR Instruction to return from interruption
	 */
	reti();
}

void xInitTaskScheduler() {

	cli();

	/** 
	 * Enable timer mode to CTC
	 */
	TCCR2A |= (1 << WGM21);

	/**
	 * 1ms delay (using 16mhz clock freq and 64 prescale)
	 */
	OCR2A = configTASK_SCHEDULER_TMR;

	/**
	 * Set interrupt vector to timer0
	 */
	TIMSK2 |= (1 << OCIE2A); 

	/**
	 * Set prescale to 64
	 */
	TCCR2B |= (1 << CS22);

	/**
	 * Clear the __sysTask
	 */
	memset(__sysTask, 0x0, sizeof(node_t) * configMAX_SYS_TASK);

	/**
	 * Enable interrupts
	 */
	sei();

}

BaseType_t xTaskCreate( TaskFunction_t pvTaskCode,
						const char * const pcName,
						unsigned short usStackDepth,
						void *pvParameters,
						UBaseType_t uxPriority,
						TaskHandle_t *pxCreatedTask
					) {

	static unsigned short gStack = RAMEND - configMAX_TASK_STACK_SIZE;

	/**
	 * Allocate a new task
	 */
	node_t *newTask = allocNode();

	/**
	 * Check if it return NULL, if yes, it has already the max
	 * number of task, so we must return an error.
	 */
	if (newTask == NULL) return TASK_CREATE_INVALID_TASK;

	/**
	 * Copy the name
	 */
	strcpy(newTask->task.name, pcName);

	/**
	 * Check if the entry-point is valid, otherwise we must 
	 * return an error.
	 */
	if (pvTaskCode == NULL) return TASK_CREATE_INVALID_FUNCTION;

	/**
	 * Set priority, state and entrypoint and the next task to null
	 */

	newTask->task.priority   = uxPriority;
	newTask->task.state      = TASK_STATUS_READY;
	newTask->id              = 0x1;
	newTask->next            = NULL;

	/**
	 * Inser the task into the List
	 */
	insertNewTask(newTask);

	if (pxCreatedTask != NULL) {
		*pxCreatedTask = newTask->id;
	}

	/** 
	 * Check if the stakc depth is big than max stack size per task
	 */	
	if (usStackDepth > configMAX_TASK_STACK_SIZE) 
		return TASK_CREATE_INVALID_STACK;

	/**
	 * PC (16 bits)
	 */
	*(UBaseType_t*)(gStack)    = (UBaseType_t)((unsigned short)(pvTaskCode) & 0xFF);
	*(UBaseType_t*)(gStack-1)  = (UBaseType_t)((unsigned short)(pvTaskCode) >> 0x8);
	
	/**
	 * R0, SREG, R1
	 */
	*(UBaseType_t*)(gStack-2)  = 0x0;
	*(UBaseType_t*)(gStack-3)  = 0x80;
	*(UBaseType_t*)(gStack-4)  = 0x0;

	/**
	 * R2 ~ R31
	 */
	for (int i = 5; i < 35; i++) {
		*(UBaseType_t*)(gStack-i) = 0x0;
	}

	/**
	 * TODO: I need to read how to pass the argument to the funciton
	 * in this case, the calling convention
	 */

	/**
	 * Configure stack location
	 */
	newTask->task.SP_ctx[0] = (UBaseType_t)((gStack-34) & 0xFF);
	newTask->task.SP_ctx[1] = (UBaseType_t)((gStack-34) >> 0x8);

	/**
	 * Decrease the stack to the next task
	 */
	gStack = gStack - usStackDepth;


	return TASK_OK;
}



void vTaskDelete ( TaskHandle_t xTask ) {
	removeTask (xTask);
}


void vTaskDelay (const UBaseType_t ms) {
	cli();

	c_Task->tmr = ms;
	c_Task->state = TASK_STATUS_SUSPENDED;
	
	sei();

	/**
	 * I'm not sure if this gonan work, but it should JMP direct to 
	 * interrupt vector
	 */
	//asm volatile("JMP TIMER2_COMPA_vect");
}

#include "scheduler.h"
#include "serial.h"

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
	/**
	 * TODO: Write a better function
	 * i.e: better suspended state detection
	 */
	node_t *hPriority = taskList;
	node_t *nPriority = NULL;

	if (hPriority->next == NULL) {
		return &hPriority->task;
	}

	nPriority = hPriority->next;

	while (1) {
		/**
		 * Sleep task
		 */
		if (hPriority->task.state == TASK_STATUS_SUSPENDED) {
			hPriority->task.tmr--;

			if (!hPriority->task.tmr) {
				hPriority->task.state = TASK_STATUS_READY;
			} else {
				hPriority = nPriority;

				if (hPriority->next == NULL) {
					return &hPriority->task;
				}

				nPriority = hPriority->next;
			}
		} else {
			if (nPriority->task.priority > hPriority->task.priority) {
				if (nPriority->task.state == TASK_STATUS_SUSPENDED) {
					nPriority->task.tmr--;

					if (!nPriority->task.tmr) {
						nPriority->task.state = TASK_STATUS_READY;
						hPriority = nPriority;
					}

				} else {
					hPriority = nPriority;
				}
			}
	
			if (nPriority->next != NULL) {
				nPriority = nPriority->next;
			} else {
				return &hPriority->task;
			}
		}

	}


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
	 * Restore the current high priority task context
	 */
	portRESTORE_CONTEXT();

	//usart_send(SPL);
	//usart_send(SPH);
	//register char test asm("15");
	//usart_send(test + 35);
	//asm volatile("POP r15");
	//usart_send(test + 35);
	//while (1){}
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

	static UBaseType_t* gStack = (UBaseType_t*)(RAMEND - configMAX_TASK_STACK_SIZE);

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
	 * Check if the stakc depth is big than max stack size per task
	 */	
	if (usStackDepth > configMAX_TASK_STACK_SIZE) 
		return TASK_CREATE_INVALID_STACK;

	/**
	 * Decrease the stack to the next task
	 */
	*gStack = *gStack - usStackDepth;
	/**
	 * PC (16 bits)
	 */
	unsigned short entry_ptr = (unsigned short)(pvTaskCode);

	*(UBaseType_t*)(gStack)  = (UBaseType_t)(entry_ptr & 0xFF);
	gStack--;
	entry_ptr = entry_ptr >> 0x8;
	*(UBaseType_t*)(gStack)  = (UBaseType_t)(entry_ptr & 0xFF);
	gStack--;
	/**
	 * R0, SREG, R1
	 */
	*(UBaseType_t*)(gStack)  = 0x0;     // R0
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x80;    // SREG
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R1
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R2
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R3
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R4
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R5
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R6
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R7
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R8
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R9
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R10
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R11
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R12
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R13
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R14
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R15
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R16
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R17
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R18
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R19
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R20
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R21
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R22
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R23
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R24
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R25
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R26
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R27
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R28
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R29
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R30
	gStack--;
	*(UBaseType_t*)(gStack)  = 0x0;     // R31
	gStack--;

	/**
	 * TODO: I need to read how to pass the argument to the funciton
	 * in this case, the calling convention
	 */

	/**
	 * Configure stack location
	 */

	newTask->task.SP_ctx = gStack;

	/**
	 * Inser the task into the List
	 */
	insertNewTask(newTask);


	if (pxCreatedTask != NULL) {
		*pxCreatedTask = newTask->id;
	}

	return TASK_OK;
}



void vTaskDelete ( TaskHandle_t xTask ) {
	removeTask (xTask);
}


void vTaskDelay (const unsigned short ms) {
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

#include "scheduler.h"


/**
 * Heap Memory Array used to heap allocation
 */
static node_t __sysTask[configMAX_SYS_TASK];

/**
 * Global current task stack pointer
 */
volatile unsigned short* pxCurrentTCB;


/**
 * Task list head
 */
node_t *taskList = NULL;

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


inline node_t* findHighPriorityTask() {
	node_t* aux = taskList;

	/**
	 * Check if the next is NULL, if yes, the root that has 
	 * the high priority
	 */

	while (1) {
		if (aux->task->priority == TASK_STATUS_SUSPENDED) {
			if (aux->next != NULL) {
				aux = aux->next;
			} else {
				return NULL;
			}
		}

		if (aux->next == NULL) break;

		if (aux->next->task.priority > aux->task.priority) {
			if (aux->next->task.state == TASK_STATUS_READY)
				aux = aux->next;
		}
	}


	return aux;
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


	node_t* aux = findHighPriorityTask();

	/**
	 * Verify if it return != NULL, otherwise we won't do nothing
	 */
	if (aux == NULL) {
		
	} else {

	}

	/**
	 * AVR Instruction to return from interruption
	 */
	asm volatile("RETI");
}

BaseType_t xInitTaskScheduler() {

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

	return 0;
}

BaseType_t xTaskCreate( TaskFunction_t pvTaskCode,
						const char * const pcName,
						unsigned short usStackDepth,
						void *pvParameters,
						UBaseType_t uxPriority,
						TaskHandle_t *pxCreatedTask
					) {

	/**
	 * Allocate a new task
	 */
	node_t *newTask = allocNode();

	/**
	 * Check if it return NULL, if yes, it has already the max
	 * number of task, so we must return an error.
	 */
	if (newTask == NULL) return TASK_CREATE_INVALID_TASK;

	strcpy(newTask->task.name, pcName);

	/**
	 * Check if the entry-point is valid, otherwise we must 
	 * return an error.
	 */
	if (pvTaskCode == NULL) return TASK_CREATE_INVALID_FUNCTION;

	/**
	 * Set priority, state and entrypoint and the next task to null
	 */

	newTask->task.priority   = uxPriority
	newTask->task.state      = TASK_STATUS_READY;
	newTask->task.entryPoint = pvTaskCode;
	newTask->id              = 0x1;
	newTask->next            = NULL;

	/**
	 * Inser the task into the List
	 */
	insertNewTask(newTask);

	if (pxCreatedTask != NULL) {
		*pxCreatedTask = task->id;
	}
}



void vTaskDelete ( TaskHandle_t xTask ) {
	removeTask (xTask);
}
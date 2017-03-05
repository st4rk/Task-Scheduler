#include "scheduler.h"


/**
 * Heap Memory Array used to heap allocation
 */
static unsigned char __heapmem[configMAX_HEAP_LEN];

/**
 * Task list head
 */

node_t *taskList = NULL;

/**
 * Embedded malloc to scheduler
 * TODO: Improve the code
 */
void* malloc(unsigned char size) {
	static unsigned char heapPos = 0x00;

	if (heapPos > configMAX_HEAP_LEN) {
		return 0;
	} else if ((heapPos + size) > configMAX_HEAP_LEN) {
		return 0;
	} else {
		unsigned char oldPos = heapPos;
		heapPos += size;
		return __heapmem[oldPos];		
	}

	return 0;
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
			/**
			 * TODO: How to write the removeTask
			 * The problem is that there is no mem management
			 * even if I clear teh memory, the malloc won't
			 * detect this memory block, so to write it
			 * I need to write a better malloc  
			 */

			 break;
		}

		if (aux->next == NULL) break;

		aux = aux->next;
	}
}


/**
 * This function is the timer0 compare interrupt handle
 * everytime that the timer reach the desired value
 * this interrupt handler will be triggered.
 * Here we will write our task scheduler that will
 * check which task is ready and that task has high-priority
 * and start it
 */
ISR (TIMER0_COMPA_vect) {
	
}

BaseType_t xInitTaskScheduler() {


	/**
	 * Calculate timer OCRn value
	 * OCRx = [(MCU_FREQ/PreScale) * time_in_sec] - 1
	 */

	/** 
	 * Enable timer mode to CTC
	 */
	TCCR0A |= (1 << WGM01);

	/**
	 * 1ms delay (using 16mhz clock freq and 64 prescale)
	 */
	OCR0A = 249;

	/**
	 * Set interrupt vector to timer0
	 */
	TIMSK0 |= (1 << OCIE0A); 

	/**
	 * Set prescale to 64
	 */
	TCCR0B |= (1 << CS01) | (1 << CS00);

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
	node_t *newTask = (node_t) malloc (sizeof(node_t));


	/**
	 * Copy the task name
	 */
	unsigned char i = 0x0;
	while (pcName[i] != '\0') { 
		if (i > 8) { 
			break; 
		} else {
			newTask->task.name = pcName[i];
		}
	}

	/**
	 * Set priority, state and entrypoint and the next task to null
	 */
	newTask->task.priority   = uxPriority
	newTask->task.state      = TASK_STATUS_READY;
	newTask->task.entryPoint = pvTaskCode;
	newTask->id              = 0x0;
	newTask->next = NULL;

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
#include "scheduler.h"


/** 
 * Idle Task
 */
void vIdleTask( void *args ) {

	while (1) {
		Serial.println("well, I'm the idle task :^)");
	}
}


/**
 * Spawn Task
 */
void vSpawnTask( void *args ) {
	
	while (1) {

	 	Serial.println("yay, I'm spawn task!");
		/** 
		 * Sleep the task for 100 ms
		 */
		//vTaskDelay(100);
	}
}


void setup() {
	
  /**
   * Initialize the Serial
   */
  Serial.begin(9600);
  
  /**
   * Create a new task (idle)
   */
  BaseType_t result = xTaskCreate ( vIdleTask, "idle", 0x40, (void*)0x0, 
  									tskIDLE_PRIORITY, NULL);

  
  result = xTaskCreate ( vSpawnTask, "spawn", 0x40, (void*)0x0, 
  	  						 tskIDLE_PRIORITY+5, NULL);

  /**
   * Start the task scheduler
   */
//  xInitTaskScheduler();
  Serial.println("ohayo nii chan");
}

void loop() {
  // put your main code here, to run repeatedly:

}

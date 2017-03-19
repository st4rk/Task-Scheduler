#include "scheduler.h"
#include "serial.h"

/** 
 * Idle Task
 */
void vIdleTask( void *args ) {
  
  while (1) {
    Serial.print('C');
    delay(5000);
  }
}


/**
 * Spawn Task
 */
void vSpawnTask( void *args ) {
  
  while (1) {
    Serial.print('E');
    delay(5000);

    /** 
     * Sleep the task for 100 ms
     */
    vTaskDelay(1000);
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
  xInitTaskScheduler();
  
}

void loop() {

}


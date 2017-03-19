#include "scheduler.h"
#include "serial.h"

/** 
 * Idle Task
 */
void vIdleTask( void *args ) {
  
  while (1) {
    Serial.print('C');
    digitalWrite(4, HIGH);
    delay(5000);
  }
}


/**
 * Spawn Task
 */
void vSpawnTask( void *args ) {
  
  while (1) {
    Serial.print('E');
    digitalWrite(3, HIGH);
    /** 
     * Sleep the task for 100 ms
     */
    vTaskDelay(500);
    digitalWrite(3, LOW);

  }
}



void setup() {
  /**
   * Initialize the Serial
   */
  Serial.begin(9600);

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

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


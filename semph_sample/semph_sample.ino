#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

TaskHandle_t TaskBlink_Handler;
TaskHandle_t TaskButton_Handler;
TaskHandle_t TaskSerial_Handler;

QueueHandle_t InterTaskQueue;
SemaphoreHandle_t SerialSemaphore;

void TaskBlink(void *pvParameters);
void TaskButton(void *pvParameters);
void TaskSerial(void *pvParameters);

void TaskSerial(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    while (Serial.available() > 0) {
      String str = Serial.readString();
      if (str[0] == 's') {
        vTaskSuspend(TaskBlink_Handler);
        Serial.println("TaskBlink suspended");
      } else if (str[0] == 'r') {
        vTaskResume(TaskBlink_Handler);
        Serial.println("TaskBlink resumed");
      } else {
        int val = str.toInt();
        xQueueSend(InterTaskQueue, &val, portMAX_DELAY);
      }
      vTaskDelay(1);
    }
  }
}

void TaskButton(void *pvParameters) {
  (void) pvParameters;
  const int DIN_PIN = 7;
  int value;

  pinMode(DIN_PIN, INPUT_PULLUP);

  for (;;) {
    value = digitalRead(DIN_PIN);
    if (value == LOW) {
      // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
      if ( xSemaphoreTake(SerialSemaphore, (TickType_t)5) == pdTRUE) {
        Serial.println("Button Pushed");
        delay(1500);
        xSemaphoreGive(SerialSemaphore); //Give(Free) the serial port for other applications
      }
    }
    vTaskDelay(1); //1 tick delay(15ms)
  }
}

void TaskBlink(void *pvParameters) {
  (void) pvParameters;
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) {
    int count;
    if (xQueueReceive(InterTaskQueue, &count, portMAX_DELAY) == pdPASS) {
      for (int i=0; i<count; i++) {
        if (xSemaphoreTake(SerialSemaphore, (TickType_t)5) == pdTRUE) {
          char buf[10];
          sprintf(buf, "N=%d", i+1);
          Serial.println(buf);
          xSemaphoreGive(SerialSemaphore);
        }
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);

  while (!Serial); //wait for serial communication begins

  if (SerialSemaphore == NULL) {
    SerialSemaphore = xSemaphoreCreateMutex(); //シリアル回線のセマフォ
    if ( (SerialSemaphore) != NULL ) {
      xSemaphoreGive( SerialSemaphore );
    }
  }

  InterTaskQueue = xQueueCreate(1, //queue length
                                sizeof(int)); //queue item size

  if (InterTaskQueue != NULL) {
    xTaskCreate(TaskBlink, "Blink", 128, NULL, 2, &TaskBlink_Handler); 
    xTaskCreate(TaskButton, "Button", 128, NULL, 1, &TaskButton_Handler);
    xTaskCreate(TaskSerial, "Serial", 128, NULL, 0, &TaskSerial_Handler);

    vTaskStartScheduler();
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}

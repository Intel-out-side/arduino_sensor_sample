#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#define X_OUT A0
#define Y_OUT A1
#define Z_OUT A2
#define ANALOG_MIN 0
#define ANALOG_MAX 1023
#define ZEROG_OFFSET 1600.0

TaskHandle_t TaskSensor_Handler;
TaskHandle_t TaskSerial_Handler;

QueueHandle_t InterTaskQueue;
SemaphoreHandle_t SerialSemaphore;

bool isOn = false;

void TaskSensor(void *pvParameters);
void TaskSerial(void *pvParameters);
double getAcc(double voltage);

/*
 * ADXL335アナログ出力センサ
 */
double getAcc(double voltage) {
  double acc;
  acc = 3.0 + (voltage - (ZEROG_OFFSET + 900.0)) / 300.0;

  if (acc > 3.0) {
    return 3.0;
  }
  else if (acc < -3.0) {
    return -3.0;
  }
  return acc;
}

void TaskSensor(void *pvParameters){
  (void) pvParameters;

  TickType_t LastTime1;
  TickType_t interval = 10;

  for (;;) {
    int xRaw, yRaw, zRaw;
    double xMilliVolt, yMilliVolt, zMilliVolt;
    double xg, yg, zg;
  
    xRaw = analogRead(X_OUT);
    yRaw = analogRead(Y_OUT);
    zRaw = analogRead(Z_OUT);
  
    xMilliVolt = (double)(5.0 * xRaw / ANALOG_MAX * 1000.0);
    yMilliVolt = (double)(5.0 * yRaw / ANALOG_MAX * 1000.0);
    zMilliVolt = (double)(5.0 * zRaw / ANALOG_MAX * 1000.0);
  
    xg = getAcc(xMilliVolt);
    yg = getAcc(yMilliVolt);
    zg = getAcc(zMilliVolt);
  
    xQueueSend(InterTaskQueue, &xg, portMAX_DELAY);
    xQueueSend(InterTaskQueue, &yg, portMAX_DELAY);
    xQueueSend(InterTaskQueue, &zg, portMAX_DELAY);

    if (!isOn) {
      digitalWrite(2, HIGH);
      isOn = true;
    } else {
      digitalWrite(2, LOW);
      isOn = false;
    }

    LastTime1 = xTaskGetTickCount();
    vTaskDelayUntil(&LastTime1, 10/portTICK_PERIOD_MS);

  }
}

void TaskSerial(void *pvParameters) {
  (void) pvParameters;
  double xg, yg, zg;
  
  for (;;) {
    if ( (xQueueReceive(InterTaskQueue, &xg, portMAX_DELAY) == pdPASS) &&
         (xQueueReceive(InterTaskQueue, &yg, portMAX_DELAY) == pdPASS) &&
         (xQueueReceive(InterTaskQueue, &zg, portMAX_DELAY) == pdPASS)) {

      if (xSemaphoreTake(SerialSemaphore, (TickType_t)5) == pdTRUE) {
        Serial.print(xg);
        Serial.print("\t");
        Serial.print(yg);
        Serial.print("\t");
        Serial.println(zg);
        xSemaphoreGive(SerialSemaphore);
      }
    }
  }
}

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(19200);

  // serial initialization
  while (!Serial);

  if (SerialSemaphore == NULL) {
      SerialSemaphore = xSemaphoreCreateMutex(); //シリアル回線のセマフォ
      if ( SerialSemaphore != NULL ) {
        xSemaphoreGive( SerialSemaphore ); //明示的にgiveする必要があるらしい
    }
  }

  InterTaskQueue = xQueueCreate(20, sizeof(float)); // floatを入れられるキューを20個用意する

  if (InterTaskQueue != NULL) {
    xTaskCreate(TaskSensor, "Blink", 128, NULL, 2, &TaskSensor_Handler);
    xTaskCreate(TaskSerial, "Serial", 128, NULL, 1, &TaskSerial_Handler);

    vTaskStartScheduler();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

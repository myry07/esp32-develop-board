#include <Arduino.h>

const TickType_t frequency = 3000;

void testTask(void *param)
{
  TickType_t startTime = xTaskGetTickCount();
  while (1)
  {
    Serial.printf("starttime: %d \n", startTime);
    vTaskDelayUntil(&startTime, frequency);
  }
}

void setup()
{
  Serial.begin(115200);
  xTaskCreate(testTask, "test", 1024, NULL, 1, NULL);
}

void loop()
{
}

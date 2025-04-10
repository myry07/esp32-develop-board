#include <Arduino.h>

typedef struct data
{
  int num1;
  float num2;
} data;

SemaphoreHandle_t xMutex = NULL;
int timeout = 2000;

void writeTask(void *param)
{
  data *dp = (data *)param;
  int n1 = dp->num1;
  float n2 = dp->num2;
  while (1)
  {
    if (xSemaphoreTake(xMutex, timeout) == pdPASS)
    {
      n1--;
      n2 -= 0.7;

      Serial.printf("WriteTask num1: %d  num2: %f \n", n1, n2);

      dp->num1 = n1;
      dp->num2 = n2;
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void readTask(void *param)
{
  data *dp = (data *)param;
  int n1;
  float n2;

  while (1)
  {
    if (xSemaphoreTake(xMutex, timeout) == pdPASS)
    {
      n1 = dp->num1;
      n2 = dp->num2;
      Serial.printf("ReadTask num1: %d  num2: %f \n", n1, n2);
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  data d1 = {100, 50.5};

  xMutex = xSemaphoreCreateMutex();

  xTaskCreate(writeTask, "write", 1024 * 2, (void *)&d1, 1, NULL);
  xTaskCreate(readTask, "read", 1024 * 2, (void *)&d1, 1, NULL);
}

void loop()
{
}

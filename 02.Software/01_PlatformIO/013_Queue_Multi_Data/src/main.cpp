#include <Arduino.h>

typedef struct sensor
{
  byte ID;
  float value1;
  float value2;
} senor;

QueueHandle_t queueMsg = xQueueCreate(8, sizeof(sensor));

void sensor1(void *param)
{
  sensor s1 = {1, 20.3, 100.7};

  while (1)
  {
    if (xQueueSend(queueMsg, &s1, portMAX_DELAY) == pdTRUE)
    {
      Serial.println("sensor 1 sent");
    }
    vTaskDelay(5000);
  }
}

void LCD(void *param)
{
  sensor s;
  while (1)
  {
    if (xQueueReceive(queueMsg, &s, portMAX_DELAY) == pdTRUE)
    {
      Serial.printf("Sensor %u, value1: %f, value2: %f \n", s.ID, s.value1, s.value2);
    }
    vTaskDelay(1000);
  }
}

void setup()
{
  Serial.begin(115200);
  xTaskCreate(sensor1, "s1", 1024 * 3, NULL, 3, NULL);
  xTaskCreate(LCD, "LCD", 1024 * 3, NULL, 2, NULL);
}

void loop()
{
}
#include <Arduino.h>

QueueHandle_t queueMsg = xQueueCreate(8, sizeof(char[20]));

void userA(void *param)
{
  char sent[] = "A: hello I'm A";

  while (1)
  {
    TickType_t timeout = 20;
    if (xQueueSend(queueMsg, &sent, timeout) == pdTRUE)
    {
      Serial.println("A sent");
    }
    vTaskDelay(5000);
  }
}

void userB(void *param)
{
  char sent[] = "B: hello I'm B";

  while (1)
  {
    TickType_t timeout = 20;
    if (xQueueSend(queueMsg, &sent, timeout) == pdTRUE)
    {
      Serial.println("B sent");
    }
    vTaskDelay(5000);
  }
}


void userC(void *param)
{
  char sent[] = "C: hello I'm C";

  while (1)
  {
    TickType_t timeout = 20;
    if (xQueueSend(queueMsg, &sent, timeout) == pdTRUE)
    {
      Serial.println("C sent");
    }
    vTaskDelay(5000);
  }
}

void lcdTask(void *param)
{
  char receive[20];

  while (1)
  {
    TickType_t timeout = portMAX_DELAY;
    if (xQueueReceive(queueMsg, &receive, timeout) == pdTRUE)
    {
      Serial.println(receive);
    }
    vTaskDelay(1000);
  }
}

void setup()
{
  Serial.begin(115200);
  xTaskCreate(userA, "A", 1024, NULL, 3, NULL);
  xTaskCreate(userB, "B", 1024, NULL, 3, NULL);
  xTaskCreate(userC, "C", 1024, NULL, 3, NULL);
  xTaskCreate(lcdTask, "LCD", 1024, NULL, 2, NULL);
}

void loop()
{
}

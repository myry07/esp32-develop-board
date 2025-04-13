#include <Arduino.h>

char constomA[3] = "A";
char constomB[3] = "B";
char constomC[3] = "C";

char product[10] = "espressif";

const uint32_t max_constom = 3;

SemaphoreHandle_t semaPhore;

void producer(void *param)
{
  char pd[10];
  strcpy(pd, (char *)param);

  while (1)
  {
    xSemaphoreGive(semaPhore);
    Serial.printf("%s hat product 1! \r\n", pd);
    vTaskDelay(8000);
  }
}

void constom(void *param)
{
  char st[3];
  strcpy(st, (char *)param);

  while (1)
  {
    if (xSemaphoreTake(semaPhore, 3000) == pdTRUE)
    {
      Serial.printf("%s got 1! \r\n", st);
    }
    vTaskDelay(2000);
  }
}

void setup()
{
  Serial.begin(115200);

  semaPhore = xSemaphoreCreateCounting(max_constom, 0);
  xTaskCreate(producer, "product", 1024 * 3, (void *)product, 3, NULL);
  xTaskCreate(constom, "constomA", 1024 * 3, (void *)constomA, 2, NULL);
  xTaskCreate(constom, "constomB", 1024 * 3, (void *)constomB, 2, NULL);
  xTaskCreate(constom, "constomC", 1024 * 3, (void *)constomC, 2, NULL);
}

void loop()
{
}

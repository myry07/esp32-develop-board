#include <Arduino.h>
#include "esp_task_wdt.h"

void demo0(void *param)
{
  while (1)
  {
  }
}

void demo1(void *param)
{
  while (1)
  {
  }
}

void setup()
{
  Serial.begin(115200);

  esp_task_wdt_add(NULL);
  // xTaskCreatePinnedToCore(demo0, "demo0", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(demo1, "demo1", 1024, NULL, 2, NULL, 1);
}

void loop()
{
  esp_task_wdt_reset();
  Serial.println("test");
  vTaskDelay(1000);
}

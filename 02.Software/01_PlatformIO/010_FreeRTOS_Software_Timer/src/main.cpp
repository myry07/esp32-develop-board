#include <Arduino.h>

TimerHandle_t lock;

void lockCallBack(TimerHandle_t th)
{
  Serial.println("locked");
}

void lockTask(void *param)
{
  while (1)
  {
    if(xTimerStart(lock, 3000) ==pdPASS) {
      Serial.println("finish");
    }
    vTaskDelay(1000);
  }
}

void setup()
{
  Serial.begin(115200);
  lock = xTimerCreate("lock the car", 2000, pdFALSE, (void *)0, lockCallBack);
  xTaskCreate(lockTask, "lock", 1024, NULL, 1, NULL);
}

void loop()
{
}

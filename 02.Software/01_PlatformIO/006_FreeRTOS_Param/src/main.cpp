#include <Arduino.h>

const byte LED_PIN = 2;

void GPIOInit_Output(byte num)
{
  *(volatile unsigned int *)GPIO_ENABLE_REG |= (1 << num);
  *(volatile unsigned int *)GPIO_OUT_REG |= (1 << num);
  Serial.printf("IO %d is init", num);
  delay(10);
}

void blinkTask(void *param)
{
  byte LED = *(byte *)param;
  GPIOInit_Output(LED);
  while (1)
  {
    *(volatile unsigned int *)GPIO_OUT_W1TS_REG |= (1 << LED);
    vTaskDelay(500);
    *(volatile unsigned int *)GPIO_OUT_W1TC_REG |= (1 << LED);
    vTaskDelay(500);
  }
}

void setup()
{
  void *vpt = (void *)&LED_PIN;

  xTaskCreate(blinkTask, "blink", 1024 * 2, vpt, 1, NULL);
}

void loop()
{
}

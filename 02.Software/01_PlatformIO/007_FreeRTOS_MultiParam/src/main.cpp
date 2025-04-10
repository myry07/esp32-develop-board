#include <Arduino.h>

typedef struct st
{
  byte LED_PIN;
  unsigned int time;
} st, *pst;

pst p1 = NULL;

void GPIOInit_Output(byte num)
{
  *(volatile unsigned int *)GPIO_ENABLE_REG |= (1 << num);
  *(volatile unsigned int *)GPIO_OUT_REG |= (1 << num);
  Serial.printf("IO %d is init", num);
  delay(10);
}

void blinkTask(void *param)
{
  pst p = (pst)param;
  byte pin = p->LED_PIN;
  int time = p->time;

  GPIOInit_Output(pin);
  while (1)
  {
    *(volatile unsigned int *)GPIO_OUT_W1TS_REG |= (1 << pin);
    vTaskDelay(time);
    *(volatile unsigned int *)GPIO_OUT_W1TC_REG |= (1 << pin);
    vTaskDelay(time);
  }
}

void setup()
{
  st st1 = {2, 100};
  xTaskCreate(blinkTask, "blink", 1024 * 3, &st1, 1, NULL);
}

void loop()
{
}
#include <Arduino.h>

#define button_pin 0
#define led_pin 2

volatile bool buttonReady = true;
bool ledstate = true;

TimerHandle_t debounce;
SemaphoreHandle_t semaLED = NULL;

void GPIOInit_Input(int num)
{
  *(volatile unsigned int *)GPIO_ENABLE_REG &= ~(1 << num);
  Serial.printf("IO %d is init", num);
  delay(10);
}

void GPIOInit_Output(int num)
{
  *(volatile unsigned int *)GPIO_ENABLE_REG |= (1 << num);
  *(volatile unsigned int *)GPIO_OUT_REG |= (1 << num);
  Serial.printf("IO %d is init", num);
  delay(10);
}

void timerCallBack(TimerHandle_t th)
{
  Serial.println("timer started");
  buttonReady = true;
}

void blinkTask(void *param)
{
  GPIOInit_Output(led_pin);
  *(volatile unsigned int *)GPIO_OUT_W1TC_REG |= (1 << led_pin);
  while (1)
  {
    if (xSemaphoreTake(semaLED, 3000) == pdTRUE)
    {
      ledstate = !ledstate;
      if (ledstate)
      {
        *(volatile unsigned int *)GPIO_OUT_W1TS_REG |= (1 << led_pin);
      } else {
        *(volatile unsigned int *)GPIO_OUT_W1TC_REG |= (1 << led_pin);
      }
    }
    vTaskDelay(10);
  }
}

void IRAM_ATTR buttonISR()
{
  if (buttonReady)
  {
    buttonReady = false;
    xSemaphoreGiveFromISR(semaLED, NULL);
    xTimerStartFromISR(debounce, NULL);
  }
}

void setup()
{
  Serial.begin(115200);

  semaLED = xSemaphoreCreateBinary();

  GPIOInit_Input(button_pin);
  attachInterrupt(button_pin, buttonISR, FALLING);

  debounce = xTimerCreate("debounce", 100, pdFALSE, (void *)0, timerCallBack);

  xTaskCreate(blinkTask, "blink", 1024 * 3, NULL, 2, NULL);
}

void loop()
{
}
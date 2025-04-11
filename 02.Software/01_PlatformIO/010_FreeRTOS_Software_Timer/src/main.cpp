#include <Arduino.h>

#define button_pin 0
#define gpio_pin 2

TimerHandle_t lock, check;

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

void lockTask(void *param)
{
  GPIOInit_Input(button_pin);
  while (1)
  {
    uint32_t val = *(volatile unsigned int *)GPIO_IN_REG;

    if (!(val & (1 << button_pin)))
    {
      Serial.printf("GPIO %d = LOW \r\n", button_pin);

      if (xTimerStart(lock, 3000) == pdPASS)
      {
        Serial.println("finish");
      }
      vTaskDelay(500);
    }
    vTaskDelay(10);
  }
}

void blinkTask(void *param)
{
  GPIOInit_Output(gpio_pin);
  while (1)
  {
    *(volatile unsigned int *)GPIO_OUT_W1TS_REG |= (1 << gpio_pin);
    vTaskDelay(10);
  }
}

void checkCallBack(TimerHandle_t th)
{
  Serial.println("checked");
}

void lockCallBack(TimerHandle_t th)
{
  Serial.println("locked");
}

void setup()
{
  Serial.begin(115200);
  lock = xTimerCreate("lock the car", 8000, pdFALSE, (void *)0, lockCallBack);
  check = xTimerCreate("check the led", 2000, pdTRUE, (void *)1, checkCallBack);
  xTaskCreate(lockTask, "lock", 1024 * 3, NULL, 1, NULL);
  xTaskCreate(blinkTask, "blink", 1024 * 3, NULL, 1, NULL);
  xTimerStart(check, portMAX_DELAY);
}

void loop()
{
}

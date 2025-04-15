#include <Arduino.h>
#include "gpio/gpio.h"

#define BUTTON_PIN 0
#define LED_PIN 2

volatile bool buttonReady = true;
bool ledstate = true;

static TaskHandle_t blinkHandle;
static TaskHandle_t buttonHandle;

void blinkTask(void *param)
{
  GPIOInit_Output(LED_PIN);
  *(volatile unsigned int *)GPIO_OUT_W1TC_REG |= (1 << LED_PIN);
  uint32_t ulNotiVal;

  while (1)
  {
    ulNotiVal = ulTaskNotifyTake(pdTRUE, 200);

    if (ulNotiVal > 0)
    {
      ledstate = !ledstate;
      if (ledstate)
      {
        *(volatile unsigned int *)GPIO_OUT_W1TS_REG |= (1 << LED_PIN);
      }
      else
      {
        *(volatile unsigned int *)GPIO_OUT_W1TC_REG |= (1 << LED_PIN);
      }
    }
    vTaskDelay(10);
  }
}

void buttonTask(void *param)
{
  GPIOInit_Input(BUTTON_PIN);

  while (1)
  {
    uint32_t val = *(volatile unsigned int *)GPIO_IN_REG;
    vTaskDelay(200);
    if (!(val & (1 << BUTTON_PIN)))
    {
      xTaskNotifyGive(blinkHandle);
      vTaskDelay(100);
    }
    vTaskDelay(10);
  }
}

void setup()
{
  Serial.begin(115200);
  xTaskCreate(blinkTask, "blink", 1024 * 3, NULL, 2, &blinkHandle);
  xTaskCreate(buttonTask, "button", 1024 * 3, NULL, 2, &buttonHandle);
}

void loop()
{
}
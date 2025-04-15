#include <Arduino.h>
#include "gpio/gpio.h"

#define BUTTON_PIN 0

static TaskHandle_t takeHandle = NULL;
static TaskHandle_t giveHandle = NULL;

void takeTask(void *param)
{
  uint32_t notiValue;
  BaseType_t xResult;

  while (1)
  {
    xResult = xTaskNotifyWait(0x00, 0x00, &notiValue, portMAX_DELAY); // wait for notification

    if( xResult == pdPASS)
    {
      Serial.print("Task notified, notiValue: ");
      Serial.println(notiValue);
    }
    else
    {
      Serial.println("Task not notified");
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void giveTask(void *param)
{
  GPIOInit_Input(BUTTON_PIN);
  BaseType_t xResult;
  uint32_t val = *(volatile unsigned int *)GPIO_IN_REG;

  while (1)
  {
    vTaskDelay(200 / portTICK_PERIOD_MS);
    if (!(val & (1 << BUTTON_PIN)))
    {
      xResult = xTaskNotify(&takeHandle, 0, eIncrement); // noti value +1

      Serial.println(xResult == pdPASS ? "Task notified" : "Task not notified");
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);

  xTaskCreate(takeTask, "Take Task", 1024 * 4, NULL, 3, &takeHandle);
  xTaskCreate(giveTask, "Give Task", 1024 * 4, NULL, 3, &giveHandle);

}

void loop()
{
}

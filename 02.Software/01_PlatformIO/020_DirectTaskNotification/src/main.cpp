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

    if (xResult == pdPASS)
    {
      Serial.printf("Task notified, notiValue: %d \r\n", notiValue);
      Serial.print("Task notified, notiValue: ");
      Serial.println(notiValue, BIN); // Binary
    }
    else
    {
      Serial.println("Task not notified \r\n");
    }
    vTaskDelay(10);
  }
}

void giveTask(void *param)
{
  GPIOInit_Input(BUTTON_PIN);
  BaseType_t xResult;
  while (1)
  {
    uint32_t val = *(volatile unsigned int *)GPIO_IN_REG;
    vTaskDelay(200);
    if (!(val & (1 << BUTTON_PIN)))
    {
      // xResult = xTaskNotify(takeHandle, 0, eIncrement); // noti value +1
      // xResult = xTaskNotify(takeHandle, 0, eNoAction); // noti value no change
      // xResult = xTaskNotify(takeHandle, (1UL << 4UL), eSetBits);

      /* not overwrite */
      // xResult = xTaskNotify(takeHandle, (1UL << 4UL), eSetBits);
      // xResult = xTaskNotify(takeHandle, 0b111, eSetBits);

      /* overwrite */
      // xResult = xTaskNotify(takeHandle, (1UL << 4UL), eSetBits);
      // xResult = xTaskNotify(takeHandle, 0b111, eSetValueWithOverwrite);

      xResult = xTaskNotify(takeHandle, (1UL << 4UL), eSetBits);
      xResult = xTaskNotify(takeHandle, 0b111, eSetValueWithoutOverwrite);

      Serial.println(xResult == pdPASS ? "Task notified \r\n" : "Task not notified \r\n");
    }
    vTaskDelay(10);
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

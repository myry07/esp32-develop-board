#include <Arduino.h>

#define gpio_pin 2
#define button_pin 0

static byte state = 0;

void GPIOInit_Output(int num)
{
  *(volatile unsigned int *)GPIO_ENABLE_REG |= (1 << num);
  *(volatile unsigned int *)GPIO_OUT_REG |= (1 << num);
  Serial.printf("IO %d is init", num);
  delay(10);
}

void GPIOInit_Input(int num)
{
  // 确保是输入模式（清除输出使能）
  *(volatile unsigned int *)GPIO_ENABLE_REG &= ~(1 << num);
  Serial.printf("IO %d is init", num);
  delay(10);
}

void blinkTask(void *param)
{
  while (1)
  {
    if (state == 1)
    {
      *(volatile unsigned int *)GPIO_OUT_W1TS_REG |= (1 << gpio_pin);
    }
    else
    {
      *(volatile unsigned int *)GPIO_OUT_W1TC_REG |= (1 << gpio_pin);
    }
    vTaskDelay(10);
  }
}

void buttonTask(void *param)
{
  while (1)
  {
    // 读取输入寄存器
    uint32_t val = *(volatile unsigned int *)GPIO_IN_REG;

    // 判断是高电平还是低电平
    if (!(val & (1 << button_pin)))
    {
      Serial.printf("GPIO %d = LOW \r\n", button_pin);
      vTaskDelay(200);
      if (!(val & (1 << button_pin)))
      {
        state = !state;
      }
    }
    vTaskDelay(10);
  }
}

void setup()
{
  Serial.begin(115200);
  GPIOInit_Output(gpio_pin);
  GPIOInit_Input(button_pin);

  xTaskCreate(blinkTask, "Blink", 1024 * 2, NULL, 2, NULL);
  xTaskCreate(buttonTask, "Button", 1024 * 2, NULL, 1, NULL);
}

void loop()
{
}
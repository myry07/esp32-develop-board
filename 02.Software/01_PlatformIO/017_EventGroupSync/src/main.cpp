#include <Arduino.h>

#define button_pin 0
#define led_pin 2

#define add_to_cart (1 << 0) // 0001
#define payment (1 << 1)     // 0010
#define inventory (1 << 2)   // 0100
#define got_product (1 << 3) // 1000

#define all_bits 0xFFFFFF // 24bits 1
#define wait_bits add_to_cart | payment | inventory | got_product

#define timeout 2000

EventGroupHandle_t xEventPurchase;

void GPIOInit_Output(int num)
{
  *(volatile unsigned int *)GPIO_ENABLE_REG |= (1 << num);
  *(volatile unsigned int *)GPIO_OUT_REG |= (1 << num);
  Serial.printf("IO %d is init \r\n", num);
  delay(10);
}

void GPIOInit_Input(int num)
{
  *(volatile unsigned int *)GPIO_ENABLE_REG &= ~(1 << num);
  Serial.printf("IO %d is init \r\n", num);
  delay(10);
}

void payTask(void *param)
{
  EventBits_t uxBits;

  while (1)
  {
    vTaskDelay(2000);
    uxBits = xEventGroupSync(xEventPurchase, add_to_cart, wait_bits, timeout);

    if ((uxBits & wait_bits) == wait_bits)
    {
      Serial.println("payed successful");
      vTaskDelete(NULL);
      Serial.println("payment Task delete");
    }
    vTaskDelay(10);
  }
}

void invTask(void *param)
{
  EventBits_t uxBits;

  while (1)
  {
    vTaskDelay(2000);
    uxBits = xEventGroupSync(xEventPurchase, inventory, wait_bits, timeout);

    if ((uxBits & wait_bits) == wait_bits)
    {
      Serial.println("product sent");
      vTaskDelete(NULL);
      Serial.println("inventory Task delete");
    }

    vTaskDelay(10);
  }
}

void getTask(void *param)
{
  EventBits_t uxBits;

  while (1)
  {
    vTaskDelay(2000);
    uxBits = xEventGroupSync(xEventPurchase, got_product, wait_bits, timeout);
    if ((uxBits & wait_bits) == wait_bits)
    {
      Serial.println("constom got");

      xEventGroupClearBits(xEventPurchase, all_bits);
      uxBits = xEventGroupGetBits(xEventPurchase);

      Serial.print("   Reset all Bits: ");
      Serial.println(uxBits, BIN);
      vTaskDelete(NULL);
    }
    vTaskDelay(10);
  }
}

void addTask(void *param)
{
  GPIOInit_Input(button_pin);
  EventBits_t uxBits; // 24 bits 0000...0000

  while (1)
  {
    uint32_t val = *(volatile unsigned int *)GPIO_IN_REG;
    if (!(val & (1 << button_pin)))
    {
      Serial.printf("GPIO %d = LOW \r\n", button_pin);
      vTaskDelay(2000);

      uxBits = xEventGroupSync(xEventPurchase, add_to_cart, wait_bits, timeout);
      if ((uxBits & wait_bits) == wait_bits)
      {
        Serial.println("product added into cart");

        xTaskCreate(payTask, "pay", 1024 * 3, NULL, 3, NULL);
        xTaskCreate(invTask, "inv", 1024 * 3, NULL, 3, NULL);
        xTaskCreate(getTask, "get", 1024 * 3, NULL, 3, NULL);

        Serial.println("successful created 3 Tasks");
      }
    }
    vTaskDelay(10);
  }
}

void setup()
{
  Serial.begin(115200);

  xEventPurchase = xEventGroupCreate();
  xTaskCreate(addTask, "add", 1024 * 3, NULL, 3, NULL);
}

void loop()
{
}
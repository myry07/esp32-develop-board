#include <Arduino.h>

#define button_pin 0
#define led_pin 2

#define add_to_cart (1 << 0) // 0001
#define payment (1 << 1)     // 0010
#define inventory (1 << 2)   // 0100
#define got_product (1 << 3) // 1000

#define all_bits 0xFFFFFF // 24bits 1

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

    uxBits = xEventGroupWaitBits(xEventPurchase, add_to_cart, pdFALSE, pdTRUE, portMAX_DELAY);

    if (uxBits & add_to_cart)
    {
      vTaskDelay(2000);
      uxBits = xEventGroupSetBits(xEventPurchase, payment);

      Serial.println("payed successful");
      Serial.print("   Event Group Value:");
      Serial.println(uxBits, BIN);
    }
    vTaskDelay(10);
    vTaskDelete(NULL);
  }
}

void invTask(void *param)
{
  EventBits_t uxBits;

  while (1)
  {
    uxBits = xEventGroupWaitBits(xEventPurchase, add_to_cart | payment, pdFALSE, pdTRUE, portMAX_DELAY);

    if ((uxBits & add_to_cart) && (uxBits & payment))
    {
      vTaskDelay(2000);
      uxBits = xEventGroupSetBits(xEventPurchase, inventory);

      Serial.println("product sent");
      Serial.print("   Event Group Value:");
      Serial.println(uxBits, BIN);
    }

    vTaskDelay(10);
    vTaskDelete(NULL);
  }
}

void getTask(void *param)
{
  EventBits_t uxBits;

  while (1)
  {
    uxBits = xEventGroupWaitBits(xEventPurchase, add_to_cart | payment | inventory, pdFALSE, pdTRUE, portMAX_DELAY);

    if ((uxBits & add_to_cart) && (uxBits & payment) && (uxBits & inventory))
    {
      vTaskDelay(2000);
      uxBits = xEventGroupSetBits(xEventPurchase, got_product);

      Serial.println("constom got");
      Serial.print("   Event Group Value:");
      Serial.println(uxBits, BIN);

      xEventGroupClearBits(xEventPurchase, all_bits);
      uxBits = xEventGroupGetBits(xEventPurchase);

      Serial.print("   Reset all Bits: ");
      Serial.println(uxBits, BIN);
    }
    vTaskDelay(10);
    vTaskDelete(NULL);
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

      uxBits = xEventGroupSetBits(xEventPurchase, add_to_cart); // set 1
      if ((uxBits & add_to_cart))
      {
        Serial.println("product added into cart");
        Serial.print("   Event Group Value:");
        Serial.println(uxBits, BIN);

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

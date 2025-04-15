#include<Arduino.h>

void GPIOInit_Output(int num)
{
  *(volatile unsigned int *)GPIO_ENABLE_REG |= (1 << num);
  *(volatile unsigned int *)GPIO_OUT_REG |= (1 << num);
  Serial.printf("IO %d is init", num);
  delay(10);
}

void GPIOInit_Input(int num)
{
  *(volatile unsigned int *)GPIO_ENABLE_REG &= ~(1 << num);
  Serial.printf("IO %d is init", num);
  delay(10);
}
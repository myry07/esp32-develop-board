#include <Arduino.h>

int i = 20;
int *pt;

void setup() {
  Serial.begin(115200);
  pt = &i;

  Serial.printf("i= %d\r\n", i);

  *pt = 10;
  Serial.printf("i= %d\r\n", i);
  
}

void loop() {
  
}

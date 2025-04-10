#include <Arduino.h>

int add(int num) {
  return ++num;
}

int add_point(int *num) {
  return ++(*num);
}

void setup() {
  Serial.begin(115200);

  int i = 10;
  Serial.printf("i= %d\r\n", i);

  add(i);
  Serial.printf("i= %d\r\n", i);

  /* 传入变量的地址 */
  Serial.printf("i= %d\r\n", add_point(&i));

}

void loop() {
  
}

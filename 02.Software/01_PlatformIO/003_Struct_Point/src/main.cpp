#include <Arduino.h>

typedef struct student {
  char *name;
  int age;
  struct student *calssmate;
} student;

student s1 = {"jack", 10, NULL};
student s2 = {"mari", 11, NULL};

void setup() {
  Serial.begin(115200);

  s1.calssmate = &s2;
  Serial.printf("%s 's classmate is %s", s1.name, s1.calssmate->name);
}

void loop() {

}

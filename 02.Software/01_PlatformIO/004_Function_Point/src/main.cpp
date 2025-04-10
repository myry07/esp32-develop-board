#include <Arduino.h>

#define TYPE 0

void draw_lcda(void)
{
  Serial.println("LCD_A");
}

void draw_lcdb(void)
{
  Serial.println("LCD_B");
}

typedef struct product {
  int type;
  void (*draw)(void);
} product;

product p0 = {0, draw_lcda};
product p1 = {1, draw_lcdb};

product ps[] = {p0, p1};

void setup()
{
  Serial.begin(115200);

  ps[TYPE].draw();

// #ifdef LCD_TYPE_A
//   draw_lcda();
// #else
//   draw_lcdb();
// #endif


}

void loop()
{
}

#ifndef _LED_H
#define _LED_H

void led_simple_init();
void timer_channel_simple_init();
void blink_led();
void sleep_led();
void pwm_breath_task(void *param);

#endif

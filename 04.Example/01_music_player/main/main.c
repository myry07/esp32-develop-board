#include "sd_spi.h"
#include "max98357.h"
#include "button.h"
#include "led.h"

void app_main(void)
{
    led_simple_init();
    timer_channel_simple_init();
    button_init();
    sd_spi_init();
    i2s_init();
}
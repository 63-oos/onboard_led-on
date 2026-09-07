#include<stdio.h>
#include "driver/gpio.h"

#define LED_GPIO GPIO_NUM_8

void app_main()
{
    /*gpio_config_t led_config = {
        .pin_bit_mask = (1<<LED_GPIO)
        .mode = 
    }*/
   gpio_set_direction(LED_GPIO,GPIO_MODE_OUTPUT);
   gpio_set_level(LED_GPIO,0);
}

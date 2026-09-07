#include<stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_GPIO GPIO_NUM_8

void app_main()
{
    gpio_config_t led_config = {
        .pin_bit_mask = (1<<LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&led_config);

    while(1)
    {
        gpio_set_level(LED_GPIO,0);
        printf("Led on\n");

        vTaskDelay(pdMS_TO_TICKS(1000));

        gpio_set_level(LED_GPIO,1);
        printf("Led off\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

   /*gpio_set_direction(LED_GPIO,GPIO_MODE_OUTPUT);
   gpio_set_level(LED_GPIO,0);*/
}

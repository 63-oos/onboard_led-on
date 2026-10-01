/*
 * on board led testing with 
 * controlling the led using Button
 
*/

#include<stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_GPIO        GPIO_NUM_8 //on boardled 
#define BUTTON_GPIO     GPIO_NUM_9 //on board boot button

void app_main()
{
    // Configure Led as an Output
    gpio_config_t led_config = {
        .pin_bit_mask = (1<<LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&led_config);

    //configure button as input with interrnal pullup
    gpio_config_t button_config = {
        .pin_bit_mask = (1<<BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&button_config);

    int led_state = 0;
    int last_button_state = 1;

    gpio_set_level(LED_GPIO,0);

    while(1)
    {
        int button_state = gpio_get_level(BUTTON_GPIO);
        //Detect button press:High -> Low
        if (last_button_state == 1 && button_state == 0){
            led_state = !led_state;
            gpio_set_level(LED_GPIO,led_state);

            //Debounce
            vTaskDelay(pdMS_TO_TICKS(50));

        }
        last_button_state = button_state;

        //Check button ever 10 ms
        vTaskDelay(pdMS_TO_TICKS(10));
    }

   /*gpio_set_direction(LED_GPIO,GPIO_MODE_OUTPUT);
   gpio_set_level(LED_GPIO,0);*/
}

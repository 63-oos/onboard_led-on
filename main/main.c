/*
 * on board led testing with 
 * controlling the led using Button
 
*/

#include<stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_GPIO        GPIO_NUM_8 //onboard led 
#define BUTTON_GPIO     GPIO_NUM_9 //onboard boot button
#define Led_active_low  0

//Function Declarations
static void led_init(void);
static void button_init(void);
static void handle_button(void);
static void led_set(bool);

void app_main()
{
    led_init();
    button_init();

    while(1)
    {
        handle_button();
        
        //Check button ever 10 ms
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void led_init(void)
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
    led_set(true);
}

static void button_init(void)
{
    //configure button as input with interrnal pullup
    gpio_config_t button_config = {
        .pin_bit_mask = (1<<BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&button_config);
}

static void handle_button(void)
{
    static bool led_state = 1;
    static int last_button_state = 1;

    int button_state = gpio_get_level(BUTTON_GPIO);
    //Detect button press:High -> Low
    if (last_button_state == 1 && button_state == 0){
        led_state = !led_state;
        led_set(led_state);
            //Debounce
        vTaskDelay(pdMS_TO_TICKS(50));

    }
    last_button_state = button_state;
}

static void led_set(bool on)
{
    int level = Led_active_low? !on: on;
    gpio_set_level(LED_GPIO,level);
    if(level == 1)
        printf("LED->OFF\n");
    else
        printf("LED->ON\n");
}
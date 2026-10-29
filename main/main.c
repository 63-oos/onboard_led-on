/*
 * on board led testing with 
 * controlling the led using Button and user input
 * adding interrupt to the button
*/

#include<stdio.h>
#include<string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"



#define LED_GPIO        GPIO_NUM_8 //onboard led 
#define BUTTON_GPIO     GPIO_NUM_9 //onboard boot button
#define Led_Active_Low  0

static QueueHandle_t button_evt_queue;

//Function Declarations
static void console_init(void);
static void led_init(void);
static void led_set(bool);
static void button_init(void);
static void handle_button(void *pv);
static void handle_command(const char*);

static void IRAM_ATTR button_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(button_evt_queue,&gpio_num,NULL);
}


void app_main()
{
    console_init();
    led_init();
    button_init();

    printf("Ready : Type on or off and press enter...\n");

    char line[32];
    int idx = 0;
    while(1)
    {
        int c = getchar();
        if(c == EOF)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if(c == '\r'||c == '\n')
        {
            line[idx] = '\0';
            handle_command(line);
            idx = 0;
        }
        else if (c == '\b' || c == 127)//backspace/delete
        {
            if(idx>0)
            {
                idx--;
                printf("\b \b");//move back, erase ,move back again
            }
        }
        else if (idx < (int)sizeof(line) - 1) 
        {
            line[idx++] = (char)c;
            putchar(c); // <-- this is the actual echo
        }
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
        .intr_type = GPIO_INTR_DISABLE,
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
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&button_config);

    button_evt_queue = xQueueCreate(10,sizeof(uint32_t));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO,button_isr_handler,(void*)BUTTON_GPIO);

    xTaskCreate(handle_button,"handle_button",2048,NULL,10,NULL);
}

static void handle_button(void*pv)
{
    uint32_t gpio_num;
    static bool led_state = 1;
    static int last_button_state = 1;
    while(1){
        if(xQueueReceive(button_evt_queue,&gpio_num,portMAX_DELAY)){
            int button_state = gpio_get_level(BUTTON_GPIO);
            //Detect button press:High -> Low
            if (button_state == 0){
                led_state = !led_state;
                led_set(led_state);
            }
            last_button_state = button_state;
            //Debounce
            //vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

static void console_init(void)
{
    usb_serial_jtag_driver_config_t usb_cfg = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usb_cfg));
    usb_serial_jtag_vfs_use_driver();

    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    // No buffering, so every putchar()/getchar() takes effect immediately
    // instead of waiting in a buffer -- required for real-time echo.
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

}

static void handle_command(const char* line)
{
    if(strcmp(line,"on")==0)
        led_set(false);
    else if(strcmp(line,"off")==0)
        led_set(true);
    else if(strlen(line)>0)
        printf("unknown command : '%s' (try on or off)\n",line);
    else
        putchar('\n');
}

static void led_set(bool on)
{
    int level = Led_Active_Low? !on : on;
    gpio_set_level(LED_GPIO,level);
    if(level == 1)
        printf("\nLED->OFF\n");
    else
        printf("\nLED->ON\n");
}


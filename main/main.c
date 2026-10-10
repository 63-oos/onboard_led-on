/*



*/

#include<stdio.h>
#include<string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"


#define LED_GPIO GPIO_NUM_8
#define Led_Active_Low 0

void led_set(bool on)
{
    int level = Led_Active_Low ? !on : on;
    gpio_set_level(LED_GPIO,level);
}

static void console_init(void)
{
    usb_serial_jtag_driver_config_t usb_cfg = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usb_cfg));
    usb_serial_jtag_vfs_use_driver();

    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

}

void app_main()
{
    console_init();
    gpio_config_t led_config = {
        .pin_bit_mask = (1<<LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&led_config);
    led_set(false);

    printf("Ready : Type on or off and press enter...\n");

    char line[32];
    while(1)
    {
        if(fgets(line,sizeof(line),stdin)!=NULL){
            line[strcspn(line,"\r\n")] = '\0';
            if(strcmp(line,"on")==0){
                led_set(false);
                printf("LED->ON\n");
            }
            else if(strcmp(line,"off")==0){
                led_set(true);
                printf("LED->OFF\n");
            }
            else
            {
                printf("%s try on or off: \n",line);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

   /*gpio_set_direction(LED_GPIO,GPIO_MODE_OUTPUT);
   gpio_set_level(LED_GPIO,0);*/
}

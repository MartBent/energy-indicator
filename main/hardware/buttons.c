#ifndef BUTTONS_C
#define BUTTONS_C

#include "buttons.h"

void IRAM_ATTR button_isr(void* arg) {
  vTaskResume(*(TaskHandle_t*)arg);
}

void button_left_handler(void* arg)
{
    while(1){
        vTaskSuspend(button_left_task_handle);
        //erase_settings();
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}
void button_right_handler(void* arg)
{
    while(1){
        vTaskSuspend(button_right_task_handle);
        printf("Right\n");
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

void setup_buttons() {
     // Initialize the buttons
    gpio_pad_select_gpio(BUTTON_LEFT);
    gpio_set_direction(BUTTON_LEFT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_LEFT, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_LEFT, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(BUTTON_LEFT);

    // Initialize the buttons
    gpio_pad_select_gpio(BUTTON_RIGHT);
    gpio_set_direction(BUTTON_RIGHT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_RIGHT, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_RIGHT, GPIO_INTR_NEGEDGE);

    gpio_intr_enable(BUTTON_RIGHT);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(BUTTON_LEFT, button_isr, &button_left_task_handle);
    gpio_isr_handler_add(BUTTON_RIGHT, button_isr, &button_right_task_handle);

    xTaskCreate(button_left_handler, "button left isr", 2048, NULL, 5, &button_left_task_handle);
    xTaskCreate(button_right_handler, "button right isr", 2048, NULL, 5, &button_right_task_handle);
}

#endif
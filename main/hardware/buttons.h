#ifndef BUTTONS_H
#define BUTTONS_H

#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"
#include "esp_freertos_hooks.h"

#include "../memory/settings.h"

static TaskHandle_t button_left_task_handle = NULL;
static TaskHandle_t button_right_task_handle = NULL;

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35

void IRAM_ATTR button_isr(void* arg);
void button_left_handler(void* arg);
void button_right_handler(void* arg);
void setup_buttons();

#endif
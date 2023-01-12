#ifndef CLOCK_LED_C
#define CLOCK_LED_C

#include "clock_led.h"



void setup_clock_led() {

    printf("Setup led strip...\n");

    static struct led_color_t led_strip_buf_1[30];
    static struct led_color_t led_strip_buf_2[30];

    struct led_strip_t led_strip = {
        .rgb_led_type = RGB_LED_TYPE_SK6812,
        .rmt_channel = RMT_CHANNEL_0,
        .rmt_interrupt_num = 19,
        .gpio = GPIO_NUM_22,
        .led_strip_buf_1 = led_strip_buf_1,
        .led_strip_buf_2 = led_strip_buf_2,
        .led_strip_length = 30
    };

    led_strip.access_semaphore = xSemaphoreCreateBinary();

    bool led_init_ok = led_strip_init(&led_strip);
}

#endif
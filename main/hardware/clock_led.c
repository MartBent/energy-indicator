#ifndef CLOCK_LED_C
#define CLOCK_LED_C

#include "clock_led.h"


static struct led_strip_t led_strip = {
    .rgb_led_type = RGB_LED_TYPE_SK6812,
    .led_strip_length = 30
};

void setup_clock_led() {

    printf("Setup led strip...\n");

    static struct led_color_t led_strip_buf_1[30];
    static struct led_color_t led_strip_buf_2[30];

    led_strip.rmt_channel = RMT_CHANNEL_0,
    led_strip.rmt_interrupt_num = 19,
    led_strip.gpio = GPIO_NUM_22,
    led_strip.led_strip_buf_1 = led_strip_buf_1,
    led_strip.led_strip_buf_2 = led_strip_buf_2,

    led_strip.access_semaphore = xSemaphoreCreateBinary();

    led_strip_init(&led_strip);
}

void disable_clock_led() {
    led_strip_clear(&led_strip);
    led_strip_show(&led_strip);
}

//Hours since midnight
void display_hour(uint8_t hours_since_midnight) {

    struct led_color_t green_color = {
        .red = 0,
        .green = 255,
        .blue = 0
    };

    led_strip_clear(&led_strip);

    printf("Displaying data on hour %d\n", hours_since_midnight);
    
    led_strip_set_pixel_color(&led_strip, hours_since_midnight, &green_color);

    led_strip_show(&led_strip);
}

#endif
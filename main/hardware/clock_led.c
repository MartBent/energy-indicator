#ifndef CLOCK_LED_C
#define CLOCK_LED_C

#include "clock_led.h"

//Index on the hour to get the corresponding led numbers
const led_couple_t led_map[12] = {
    {
        .led_one = 21,
        .led_two = 22
    },
    {
        .led_one = 19,
        .led_two = 20
    },
    {
        .led_one = 17,
        .led_two = 18
    },
    {
        .led_one = 15,
        .led_two = 16
    },
    {
        .led_one = 14,
        .led_two = 13
    },
    {
        .led_one = 11,
        .led_two = 12
    },
    {
        .led_one = 10,
        .led_two = 9
    },
    {
        .led_one = 8,
        .led_two = 7
    },
    {
        .led_one = 6,
        .led_two = 5
    },
    {
        .led_one = 4,
        .led_two = 3
    },
    {
        .led_one = 2,
        .led_two = 1
    },
    {
        .led_one = 0,
        .led_two = 23
    },
};

static struct led_strip_t led_strip = {
    .rgb_led_type = RGB_LED_TYPE_SK6812,
    .led_strip_length = 29
};

void setup_clock_led() {

    printf("Setup led strip...\n");

    static struct led_color_t led_strip_buf_1[29];
    static struct led_color_t led_strip_buf_2[29];

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
void clock_led_display_data(uint8_t hours_since_midnight, uint8_t community_performance) {

    struct led_color_t green_color = {
        .red = 0,
        .green = 255,
        .blue = 0
    };

    led_strip_clear(&led_strip);

    printf("Displaying data on hour %d\n", hours_since_midnight);
    
    //Use the led_map to determine which leds to turn on
    led_couple_t led_couple = led_map[hours_since_midnight];

    led_strip_set_pixel_color(&led_strip, led_couple.led_one, &green_color);
    led_strip_set_pixel_color(&led_strip, led_couple.led_two, &green_color);

    //If community performance value is valid, display this amount in the last 5 LEDs on the strip
    if(community_performance < 6) {
        for(int i = 0; i < community_performance; i++) {
            led_strip_set_pixel_color(&led_strip, 24 + i, &green_color);
        }
    }

    led_strip_show(&led_strip);
}

void clock_led_animate() {
    struct led_color_t white_color = {
        .red = 255,
        .green = 255,
        .blue = 255
    };

    led_strip_clear(&led_strip);

    for(int i = 0; i < 12; i++) { 
        led_strip_set_pixel_color(&led_strip, i, &white_color);
        led_strip_set_pixel_color(&led_strip, 23-i, &white_color);
        led_strip_show(&led_strip);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    for(int i = 11; i >= 0; i--) { 
        led_strip_set_pixel_color(&led_strip, i, &white_color);
        led_strip_set_pixel_color(&led_strip, 23-i, &white_color);
        led_strip_show(&led_strip);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    led_strip_clear(&led_strip);
    led_strip_show(&led_strip);
}

#endif
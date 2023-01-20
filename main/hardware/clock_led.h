#ifndef CLOCK_LED_H
#define CLOCK_LED_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "./led_strip.h"

//In this hardware design each hour is 2 leds, this struct represents the leds of each hour. Used in the LED map to get a led_couple from an hour.
typedef struct {
    uint8_t led_one;
    uint8_t led_two;
} led_couple_t;

//Index on the hour to get the corresponding led numbers
const led_couple_t led_map[12] = {
    {
        .led_one = 21,
        .led_two = 22
    },
    {
        .led_one = 23,
        .led_two = 0
    },
    {
        .led_one = 1,
        .led_two = 2
    },
    {
        .led_one = 3,
        .led_two = 4
    },
    {
        .led_one = 5,
        .led_two = 6
    },
    {
        .led_one = 7,
        .led_two = 8
    },
    {
        .led_one = 9,
        .led_two = 10
    },
    {
        .led_one = 11,
        .led_two = 12
    },
    {
        .led_one = 13,
        .led_two = 14
    },
    {
        .led_one = 15,
        .led_two = 16
    },
    {
        .led_one = 17,
        .led_two = 18
    },
    {
        .led_one = 19,
        .led_two = 20
    },
};

//Setup the LED strip with proper drivers / RMT channel.
void setup_clock_led();

//Clears the whole LED strip.
void clear_clock_led();

//Turns on 3 LEDs with green light starting at starting_index. 3 LEDs since 30 available leds, 10 hours resolution, 30/10 = 3 LEDs per hour.
void clock_led_display_data(uint8_t hours_since_midnight, uint8_t community_performance);

void disable_clock_led();

//Plays an animation with the LED strip
void clock_led_animate();

#endif
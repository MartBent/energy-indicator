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
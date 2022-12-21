#ifndef CLOCK_LED_H
#define CLOCK_LED_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "./led_strip.h"

//Setup the LED strip with proper drivers / RMT channel.
void setup_clock_led();

//Clears the whole LED strip.
void clear_clock_led();

//Turns on 3 LEDs with green light starting at starting_index. 3 LEDs since 30 available leds, 10 hours resolution, 30/10 = 3 LEDs per hour.
void display_light(uint8_t starting_index);

#endif
#ifndef BATTERY_H
#define BATTERY_H

#include "pins.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

uint32_t battery_read_voltage();
void battery_init_led();
void battery_enable_warning_led();
void battery_disable_warning_led();
void battery_enable_ok_led();
void battery_disable_ok_led();

#endif


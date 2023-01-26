#ifndef BATTERY_C
#define BATTERY_C

#include "battery.h"

//Returns the measured battery voltage in mV using an ADC pin
uint32_t battery_read_voltage() {
    esp_adc_cal_characteristics_t adc1_chars;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11));
    return esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_4), &adc1_chars);
}

//Setup all the battery LED pins
void battery_init_led() {
    gpio_pad_select_gpio(BATTERY_WARNING_LED);
    gpio_set_direction(BATTERY_WARNING_LED, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(BATTERY_OK_LED);
    gpio_set_direction(BATTERY_OK_LED, GPIO_MODE_OUTPUT);
}

//Turn on the warning LED
void battery_enable_warning_led() {
    gpio_set_level((gpio_num_t)BATTERY_WARNING_LED, 1);

}

//Turn off the warning LED
void battery_disable_warning_led() {
    gpio_set_level((gpio_num_t)BATTERY_WARNING_LED, 0);
}

//Turn on the ok LED
void battery_enable_ok_led() {
    gpio_set_level((gpio_num_t)BATTERY_OK_LED, 1);

}

//Turn off the ok LED
void battery_disable_ok_led() {
    gpio_set_level((gpio_num_t)BATTERY_OK_LED, 0);
}
#endif
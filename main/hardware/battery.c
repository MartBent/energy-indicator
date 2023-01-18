#ifndef BATTERY_C
#define BATTERY_C

#include "battery.h"

//Returns the measured battery voltage in mV
uint32_t battery_read_voltage() {
    esp_adc_cal_characteristics_t adc1_chars;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_6, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_6));
    return esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_4), &adc1_chars);
}

void battery_init_led() {
    gpio_pad_select_gpio(BATTERY_WARNING_LED);
    gpio_set_direction(BATTERY_WARNING_LED, GPIO_MODE_OUTPUT);
}

void battery_enable_led() {
    gpio_set_level((gpio_num_t)BATTERY_WARNING_LED, 1);

}

void battery_disable_led() {
    gpio_set_level((gpio_num_t)BATTERY_WARNING_LED, 0);
}

#endif
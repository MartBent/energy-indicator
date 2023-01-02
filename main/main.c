#include "hardware/buttons.h"
#include "hardware/clock_led.h"
#include "network/wifi.h"
#include "network/http.h"
#include "esp_sleep.h"
#include "memory/solar_data.h"
#include "esp_attr.h"

RTC_DATA_ATTR solar_data_cache_t cache = {};

void print_cache() {
    for(int i = 0; i < 19; i++) {
        print_solar_data(cache.data[i]);
    }    
}
bool handle_timer_wakeup(settings_t* settings)
{
    bool result = false;
    setup_wifi();
    char* response = malloc(1000);
    setup_sta(settings);
    
    //Re-enable wifi
    start_and_connect();

    int length = 0;
    if(http_get_request("https://api.forecast.solar/estimate/watt_hours_period/52.011509/6.701236/37/0/5.67", response, &length)) {
        printf("%.*s\n", length, response);

        char* line;
        uint16_t counter = 0;

        while ((line = strtok_r(response, "\n", &response))) {
            //Line is valid if it is shorter then 30 chars
            if(strlen(line) <= 30) {

                //We're gonna assume that the API returns 10 hours of each day, so 8 AM + counter. MOD 10 since 2 days of 10 hours are returned.
                char* date_time = strtok_r(line, ";", &line); 
                int watts = atoi(strtok_r(line, ";", &line));
                strtok_r(date_time, " ", &date_time);
                int time = 8 + (counter % 10);
               
                solar_data_t line_data = {
                    .data_moment = counter < 10 ? Today : Tormorrow,
                    .hour_of_day = time,
                    .watt_hour = watts
                };

                if(counter <= 19) {
                    cache.data[counter] = line_data;
                }

                print_solar_data(line_data);
                counter++;
            }
            if(counter == 19) {
                cache.data_valid = true;
                result = true;
            }
        }      
    }
    else {
        printf("The GET request resulted in error: %d\n", length);
    }
    disable_wifi();
    free(response);
    return result;
}

void handle_sensor_wakeup() {
    printf("Handling sensor wakeup...\n");
    if(cache.data_valid) {
        print_cache();
    } else {
        printf("Data cache is empty\n");
    }
}

void handle_reset_wakeup() {
    printf("Handling reset wakeup...\n");
    vTaskDelay(300 / portTICK_PERIOD_MS);
    uint32_t counter = 0;
    while(gpio_get_level(RESET_BUTTON) == 1) {
        vTaskDelay(50 / portTICK_RATE_MS);
        ++counter;
        if(counter > 100) {
            erase_settings();
            esp_restart();
        }
    }
}

void deep_sleep(uint32_t sleep_interval_seconds) {
    //Set sleep timer and go into sleep
    printf("Going into sleep...\n");
    esp_sleep_enable_timer_wakeup(sleep_interval_seconds * 1000000);
    uint64_t pin_mask = (uint64_t)1 << RESET_BUTTON;
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(pin_mask, ESP_EXT1_WAKEUP_ANY_HIGH));
    esp_deep_sleep_start();
}

void app_main(void)
{
    setup_flash();
    setup_settings();
    //setup_clock_led();

    settings_t settings;
    bool settings_found = retrieve_settings(&settings);

    if(settings_found) {
        printf("Settings found, connecting...\n"); 
        bool data_retrieved = false;

        //Depending on how the device was started, run certain tasks
        esp_sleep_source_t wakeup_cause = esp_sleep_get_wakeup_cause();

        if(wakeup_cause == ESP_SLEEP_WAKEUP_EXT1) {
            //Check pin num
            uint64_t pin_mask = esp_sleep_get_ext1_wakeup_status();

            bool did_sensor_wakeup = (pin_mask >> SENSOR_INT) == 1;
            bool did_button_wakeup = (pin_mask >> RESET_BUTTON) == 1;

            if(did_button_wakeup) {
                handle_reset_wakeup();
            } 
            else if(did_sensor_wakeup) {
                handle_sensor_wakeup();
            }
        }
        else if (wakeup_cause == ESP_SLEEP_WAKEUP_TIMER) {
            data_retrieved = handle_timer_wakeup(&settings);
        }
        else {
            //The device should always retrieve solar data from the internet on startup.
            printf("Unknown wakeup cause / Inital startup\n");
            data_retrieved = handle_timer_wakeup(&settings);
        }
        //Sleep for only an hour if no data is retrieved. Otherswise for an whole day.
        deep_sleep(data_retrieved ? 60*60*24 : 60*60);
    } else {
        printf("Settings not found, setting up AP...\n");
        setup_access_point();
    }
    while(1) {}
}  

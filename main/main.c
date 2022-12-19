#include "hardware/buttons.h"
#include "network/wifi.h"
#include "network/http.h"
#include "esp_sleep.h"

void handle_timer_wakeup(settings_t* settings)
{
    setup_wifi();
    char* response = malloc(1000);
    setup_sta(settings);
    
    //Re-enable wifi
    start_and_connect();

    int length = 0;
    if(http_get_request("https://api.forecast.solar/estimate/watt_hours_period/52.011509/6.701236/37/0/5.67", response, &length)) {
        printf("%.*s\n", length, response);

        char * hour_data = strtok(response, "\n");

        while(hour_data != NULL) {
            char* hour = strtok(hour_data, ";");
            printf("Hour %s\n", hour);
            printf("Watthours %s\n", hour_data);
            hour_data = strtok(response, "\n");
        }

        memset(response, 0xFF, 1000);
    } else {
        printf("The GET request resulted in error: %d\n", length);
    }

    disable_wifi();
    free(response);
}


void handle_sensor_wakeup() {
    printf("Handling sensor wakeup...\n");
    //Retrieve data from memory.
    //Show data...
}

void handle_reset_wakeup() {
    printf("Handling reset wakeup...\n");
    vTaskDelay(300 / portTICK_PERIOD_MS);
    uint32_t counter = 0;
    while(gpio_get_level(BUTTON_LEFT) == 0) {
        vTaskDelay(50 / portTICK_RATE_MS);
        ++counter;
        if(counter > 100) {
            erase_settings();
            esp_restart();
        }
    }
}

void deep_sleep() {
    //Set sleep timer and go into sleep
    esp_sleep_enable_timer_wakeup(30 * 1000000);
    uint64_t pin_mask = (uint64_t)1 << BUTTON_LEFT | (uint64_t)1 << BUTTON_RIGHT;
    esp_sleep_enable_ext1_wakeup(pin_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_deep_sleep_start();
}

void app_main(void)
{
    setup_flash();
    setup_settings();
    setup_buttons();

    settings_t settings;
    bool settings_found = retrieve_settings(&settings);

    if(settings_found) {
        printf("Settings found, connecting..."); 

        //Depending on how the device was started, run certain tasks
        esp_sleep_source_t wakeup_cause = esp_sleep_get_wakeup_cause();

        if(wakeup_cause == ESP_SLEEP_WAKEUP_EXT1) {
            //Check pin num
            uint64_t pin_mask = esp_sleep_get_ext1_wakeup_status();

            bool did_sensor_wakeup = (pin_mask >> BUTTON_LEFT) == 1;
            bool did_button_wakeup = (pin_mask >> BUTTON_RIGHT) == 1;

            if(did_button_wakeup) {
                handle_reset_wakeup();
            } 
            else if(did_sensor_wakeup) {
                handle_sensor_wakeup();
            }
        }
        else if (wakeup_cause == ESP_SLEEP_WAKEUP_TIMER) {
            handle_timer_wakeup(&settings);
        }
        else {
            printf("Unknown wakeup cause / Inital startup\n");
            handle_timer_wakeup(&settings);
        }
        deep_sleep();
    } else {
        printf("Settings not found, setting up AP...\n");
        setup_access_point();
    }
    while(1) {}
}  

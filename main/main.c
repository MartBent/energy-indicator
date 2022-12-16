#include "hardware/buttons.h"
#include "network/wifi.h"
#include "network/http.h"
#include "nvs_flash.h"
#include "esp_sleep.h"

const char* csv = "'2022-12-16 08:33:00': 0,'2022-12-16 09:00:00': 20,'2022-12-16 10:00:00': 189,'2022-12-16 11:00:00': 343,'2022-12-16 12:00:00': 419,'2022-12-16 13:00:00': 440,'2022-12-16 14:00:00': 424,'2022-12-16 15:00:00': 352,'2022-12-16 16:00:00': 211,'2022-12-16 16:24:00': 25,'2022-12-17 08:34:00': 0,'2022-12-17 09:00:00': 237,'2022-12-17 10:00:00': 1464,'2022-12-17 11:00:00': 2049,'2022-12-17 12:00:00': 2323,'2022-12-17 13:00:00': 2313,'2022-12-17 14:00:00': 2026,'2022-12-17 15:00:00': 1467,'2022-12-17 16:00:00': 738,'2022-12-17 16:24:00': 71";

void setup_flash() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
}
void decode() {
    char * hour_data = strtok(csv, ",");
    while(hour_data != NULL) {
        char* hour = strtok(hour_data, ";");
        printf("Hour %s", hour);
        printf("Watthours %s", hour_data);
        hour_data = strtok(csv, "\n");
    }
}
void handle_timer_wakeup(settings_t* settings)
{
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

    //Disable wifi before going into sleep
    disable_wifi();
    free(response);
}


void handle_sensor_wakeup() {
    //Retrieve data from memory.
    //Show data...
}

void handle_reset_wakeup() {
    //Check if key is held down for 5 sec
    //Yes: reset settings No: return
}

void deep_sleep() {
    //Set sleep timer and go into sleep
    esp_sleep_enable_timer_wakeup(30 * 1000000);
    uint64_t pin_mask = (uint64_t)1 << BUTTON_LEFT | (uint64_t)1 << BUTTON_RIGHT;
    esp_sleep_enable_ext1_wakeup(pin_mask, 0);
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
            setup_wifi();
            handle_timer_wakeup(&settings);
        }
        else {
            printf("Unknown wakeup cause / Inital startup\n");
            setup_wifi();
            handle_timer_wakeup(&settings);
        }
        deep_sleep();
    } else {
        printf("Settings not found, setting up AP...\n");
        //setup_wifi();
        setup_access_point();
    }
    while(1) {}
}  

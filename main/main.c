#include "hardware/buttons.h"
#include "network/wifi.h"
#include "network/http.h"
#include "nvs_flash.h"
#include "esp_sleep.h"

void setup_flash() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
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

        //Json decode
        //Save times to flash
        
        memset(response, 0xFF, 1000);
    } else {
        printf("Error doing GET request\n");
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

        vTaskDelay(5000 / portTICK_PERIOD_MS);
        deep_sleep();
    } else {
        setup_wifi();
        setup_access_point();
    }
    while(1) {}
}  

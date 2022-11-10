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

void gather_data_task(void* arg)
{
    char* response = malloc(1000);
    setup_sta((settings_t*)arg);

    while(1){

        //Re-enable wifi
        start_and_connect();

        uint16_t length = 0;
        if(print_get("http://httpbin.org/get", response, &length)) {
            printf("%.*s", length, response);
            memset(response, 0xFF, 1000);
        } else {
            printf("Error doing GET request");
        }

        //Disable wifi before going into sleep
        disable_wifi();

        //Set sleep timer and go into sleep
        esp_sleep_enable_timer_wakeup(10 * 1000000);
        ESP_ERROR_CHECK(esp_light_sleep_start());
    }
    free(response);
}

void app_main(void)
{
    setup_flash();
    setup_settings();
    setup_buttons();
    setup_wifi();

    settings_t settings;
    bool settings_set = retrieve_settings(&settings);

    if(settings_set) {
        xTaskCreate(gather_data_task, "gather data task", 2048, &settings, 5, NULL);
    } else {
        setup_ap();
    }
    while(1) {}
}  

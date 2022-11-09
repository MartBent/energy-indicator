#include "hardware/buttons.h"
#include "network/wifi.h"
#include "network/http.h"
#include "nvs_flash.h"

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
    while(1){
        uint16_t length = 0;
        if(print_get("http://httpbin.org/get", response, &length)) {
            printf("%.*s", length, response);
            memset(response, 0xFF, 1000);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
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
    bool set = retrieve_settings(&settings);

    if(set) {
        if(setup_sta(&settings)) {
            //Start data api gathering...
            xTaskCreate(gather_data_task, "gather data task", 2048, NULL, 5, NULL);
        } else {
            //SSID invalid, reset settings.
            erase_settings();
            esp_restart();
        }
    } else {
        setup_ap();
    }

    while(1) {}
}  

#include "buttons.c"
#include "network.c"

#include "nvs_flash.h"

void setup_flash() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
}

void app_main(void)
{
    setup_flash();
    setup_settings();
    setup_buttons();
    setup_ap();

    settings_t settings;
    bool set = retrieve_settings(&settings);

    if(set) {
        printf("SSID: %.*s\n", settings.ssid_length, settings.ssid);
        printf("PASS: %.*s\n", settings.password_length, settings.password);
        printf("API: %.*s\n", settings.api_key_length, settings.api_key);
    }

    while(1) {}
}  

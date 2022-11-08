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
    setup_buttons();
    setup_ap();

    while(1) {}
}  

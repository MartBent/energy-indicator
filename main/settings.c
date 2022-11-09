#ifndef SETTINGS_C
#define SETTINGS_C

#include "esp_system.h"
#include "esp_partition.h"
#include "settings.h"
#include <string.h>

static const esp_partition_t* settings_partition;

bool retrieve_settings(settings_t* settings) {
    printf("Retrieving settings...\n");
    uint8_t code[8];

    ESP_ERROR_CHECK(esp_partition_read_raw(settings_partition, 192+3, code, 8));

    bool ok = true;

    for(int i = 0; i < 8; i++) {
        if(code[i] != SETTINGS_CODE[i]) {
            return false;
        }
    }

    ESP_ERROR_CHECK(esp_partition_read_raw(settings_partition, 0, &settings->ssid_length, 1));
    ESP_ERROR_CHECK(esp_partition_read_raw(settings_partition, 1, &settings->password_length, 1));
    ESP_ERROR_CHECK(esp_partition_read_raw(settings_partition, 2, &settings->api_key_length, 1));

    ESP_ERROR_CHECK(esp_partition_read_raw(settings_partition, 3, settings->ssid, 64));
    ESP_ERROR_CHECK(esp_partition_read_raw(settings_partition, 64+3, settings->password, 64));
    ESP_ERROR_CHECK(esp_partition_read_raw(settings_partition, 128+3, settings->api_key, 64));
    return true;
}

void save_settings(const settings_t* settings) {
    printf("Saving settings...\n");
    ESP_ERROR_CHECK(esp_partition_erase_range(settings_partition, 0, 4096));

    ESP_ERROR_CHECK(esp_partition_write_raw(settings_partition, 0, &settings->ssid_length, 1));
    ESP_ERROR_CHECK(esp_partition_write_raw(settings_partition, 1, &settings->password_length, 1));
    ESP_ERROR_CHECK(esp_partition_write_raw(settings_partition, 2, &settings->api_key_length, 1));

    ESP_ERROR_CHECK(esp_partition_write_raw(settings_partition, 3, settings->ssid, 64));
    ESP_ERROR_CHECK(esp_partition_write_raw(settings_partition, 64+3, settings->password, 64));
    ESP_ERROR_CHECK(esp_partition_write_raw(settings_partition, 128+3, settings->api_key, 64));

    ESP_ERROR_CHECK(esp_partition_write_raw(settings_partition, 192+3, SETTINGS_CODE, 8));
}

void erase_settings() {
    printf("Erasing settings..\n");
    ESP_ERROR_CHECK(esp_partition_erase_range(settings_partition, 0, 4096));
}

void setup_settings() {
    printf("Setup settings...\n");
    settings_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "settings");
}

#endif
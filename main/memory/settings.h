#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <string.h>
#include "esp_system.h"
#include "esp_partition.h"

typedef struct {
    char ssid[64];
    uint8_t ssid_length;
    char password[64];
    uint8_t password_length;
    char api_key[64];
    uint8_t api_key_length;
    uint16_t pir_counter;
} settings_t;

static const uint8_t SETTINGS_CODE[8] = {0xD,0xE,0xA,0xD,0xB,0xE,0xE,0xF};

bool retrieve_settings(settings_t* settings);

void save_settings(const settings_t* settings);

void erase_settings();

void setup_settings();

#endif
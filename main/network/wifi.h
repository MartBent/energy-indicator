#ifndef WIFI_H
#define WIFI_H

#include <esp_http_server.h>
#include <esp_check.h>
#include "esp_wifi.h"

#include "../memory/settings.h"
#include "http.h"

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void setup_wifi();
void setup_ap();
bool setup_sta(const settings_t* settings);

#endif
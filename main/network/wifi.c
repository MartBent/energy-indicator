#ifndef WIFI_C 
#define WIFI_C

#include "wifi.h"

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI("WiFi-SoftAP", "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI("WiFi-SoftAP", "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        //wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
        connected = false;
        ESP_LOGI("WiFi-STA", "Lost connection");
    }
    else if (event_id == IP_EVENT_STA_GOT_IP) {
        connected = true;
        ESP_LOGI("WiFi-STA", "Gained connected");
    }
}

void setup_wifi() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
}

void setup_access_point() {
    setup_wifi();
    esp_netif_t* wifiAP = esp_netif_create_default_wifi_ap();
    esp_netif_ip_info_t ipInfo;
    IP4_ADDR(&ipInfo.ip, 192,168,1,1);
    IP4_ADDR(&ipInfo.gw, 0,0,0,0);
    IP4_ADDR(&ipInfo.netmask, 255,255,255,0);

    esp_netif_dhcps_stop(wifiAP);
    esp_netif_set_ip_info(wifiAP, &ipInfo);
    esp_netif_dhcps_start(wifiAP);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "default",
            .ssid_len = 7,
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        },
    };

    uint8_t mac_addr[6] = {0};
    ESP_ERROR_CHECK(esp_read_mac(mac_addr, ESP_MAC_WIFI_SOFTAP));

    char ssid[32];
    sprintf(ssid, "ei-%x:%x:%x:%x:%x:%x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    memcpy(wifi_config.ap.ssid, ssid, 32);
    wifi_config.ap.ssid_len = strlen(ssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(httpd_start(&server, &config));
    httpd_uri_t settings_uri = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = access_handler,
        .user_ctx = NULL
    };

    httpd_uri_t save_uri = {
        .uri      = "/",
        .method   = HTTP_POST,
        .handler  = save_handler,
        .user_ctx = NULL
    };

    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &settings_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &save_uri));
}

bool setup_sta(const settings_t* settings) {
    printf("SSID: %.*s\n", settings->ssid_length, settings->ssid);
    printf("PASS: %.*s\n", settings->password_length, settings->password);
    printf("API: %.*s\n", settings->api_key_length, settings->api_key);

    esp_netif_create_default_wifi_sta();
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    memcpy(wifi_config.sta.ssid, settings->ssid, settings->ssid_length);
    memcpy(wifi_config.sta.password, settings->password, settings->password_length);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    return start_and_connect();
}

bool start_and_connect() {
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_err_t err = esp_wifi_connect();
    if(err == ESP_ERR_WIFI_SSID) {
        return false;
    } else {
        uint16_t delay_table[4] = {500, 1000, 2000, 5000};
        uint8_t counter = 0;
        while(counter < 4 && !connected){
            vTaskDelay(delay_table[counter++] / portTICK_PERIOD_MS);
        }
        return connected;
    }
}

void disable_wifi() {
    connected = false;
    ESP_ERROR_CHECK(esp_wifi_stop());
}

#endif
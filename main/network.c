#include <esp_http_server.h>
#include <esp_check.h>
#include "esp_wifi.h"
#include "page.h"

#include "settings.h"

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
}

esp_err_t access_handler(httpd_req_t *req)
{
    httpd_resp_send(req, page_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t save_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "DELETE, POST, GET, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");

    char ssid[64] = {};
    char password[64] = {};
    char api_key[64] = {};

    httpd_req_get_hdr_value_str(req, "SSID", ssid, 64);
    httpd_req_get_hdr_value_str(req, "PASS", password, 64);
    httpd_req_get_hdr_value_str(req, "API", api_key, 64);

    printf("SSID: %s\n", ssid);
    printf("PASS: %s\n", password);
    printf("API: %s\n", api_key);

    settings_t settings;

    settings.ssid_length = httpd_req_get_hdr_value_len(req, "SSID");
    settings.password_length = httpd_req_get_hdr_value_len(req, "PASS");
    settings.api_key_length = httpd_req_get_hdr_value_len(req, "API");

    memcpy(settings.ssid, ssid, settings.ssid_length);
    memcpy(settings.password, password, settings.password_length);
    memcpy(settings.api_key, api_key, settings.api_key_length);

    save_settings(&settings);

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void setup_ap() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_t* wifiAP = esp_netif_create_default_wifi_ap();

    esp_netif_ip_info_t ipInfo;
    IP4_ADDR(&ipInfo.ip, 192,168,1,1);
    IP4_ADDR(&ipInfo.gw, 0,0,0,0); // do not advertise as a gateway router
    IP4_ADDR(&ipInfo.netmask, 255,255,255,0);

    esp_netif_dhcps_stop(wifiAP);
    esp_netif_set_ip_info(wifiAP, &ipInfo);
    esp_netif_dhcps_start(wifiAP);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

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

void setup_sta() {
    
}

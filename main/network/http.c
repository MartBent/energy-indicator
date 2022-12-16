#ifndef HTTP_C 
#define HTTP_C

#include "http.h"

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

    //Restart ESP to get into STA mode
    esp_restart();

    return ESP_OK;
}
static char rx_data[1000] = {};
static int rx_data_len = 0;

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            //Copy to user data if it is not null
            if (evt->user_data) {
                memcpy(evt->user_data + rx_data_len, evt->data, evt->data_len);
                memcpy(rx_data + rx_data_len, evt->data, evt->data_len);
            }
            rx_data_len += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            break;
        case HTTP_EVENT_DISCONNECTED:
            break;
        default:
           break;
    }
    return ESP_OK;
}

bool http_get_request(const char* url, char* response, int* length) {
    *length = 0;
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = http_event_handler,
        .disable_auto_redirect = true,
        .user_data = response
    };  

    esp_http_client_handle_t client = esp_http_client_init(&config);

    const uint16_t delay_table[3] = {2000, 5000, 10000};

    bool ok = false;

    for(int i = 0; i < 3; i++) {
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            int status = esp_http_client_get_status_code(client);
            if(status == 200) {
                ok = true;
                break;
            } else if(status == 400) {
                vTaskDelay(delay_table[i] / portTICK_PERIOD_MS);
            } 
        } else {
            vTaskDelay(delay_table[i] / portTICK_PERIOD_MS);
        }
    }

    if(ok) {
        while(!esp_http_client_is_complete_data_received(client)) {}
        if(rx_data_len > 0) {
            *length = rx_data_len;
            memcpy(response, rx_data, rx_data_len);
            rx_data_len = 0;
        }
    }
    esp_http_client_cleanup(client);

    return ok && *length > 0;
}
#endif
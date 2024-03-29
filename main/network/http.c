#ifndef HTTP_C 
#define HTTP_C

#include "http.h"

//This handler is ran when the user does a GET request to the IP address of the ESP, the ESP will repond with the webpage which can be found in page.c
esp_err_t access_handler(httpd_req_t *req)
{
    httpd_resp_send(req, page_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

//This handler is used when the user does a POST request to the IP address of the ESP, if this POST request contains the SSID and PASSWORD field, it will be saved in flash memory and the ESP is restarted.
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

//Event handler to handle the retrieved data of an GET request.
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

//This function make a GET request to a certain url, it returns false if it was unable to do this GET request. If it returns false the length field will contain the HTTP error code.
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

    esp_http_client_set_header(client, "Accept", "text/csv");

    const uint16_t delay_table[3] = {2000, 5000, 10000};

    bool ok = false;

    int status = 0;

    for(int i = 0; i < 3; i++) {
        esp_err_t err = esp_http_client_perform(client);
        status = esp_http_client_get_status_code(client);
        if (err == ESP_OK) {
            if(status == 200) {
                ok = true;
                break;
            } else {
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
    } else {
        //Put the status in length 
        *length = status;
    }
    esp_http_client_cleanup(client);

    return ok && *length > 0;
}
#endif
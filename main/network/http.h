#ifndef HTTP_H
#define HTTP_H

#include "../memory/settings.h"
#include <esp_http_server.h>
#include "esp_http_client.h"
#include <esp_check.h>
#include "page.h"
#include "esp_tls.h"

esp_err_t access_handler(httpd_req_t *req);
esp_err_t save_handler(httpd_req_t *req);

bool print_get(const char* url, char* response, uint16_t* length);

#endif
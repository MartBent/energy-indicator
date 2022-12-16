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

bool http_get_request(const char* url, char* response, int* length);

#endif
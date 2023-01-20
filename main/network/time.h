#include <time.h>
#include <stdio.h>
#include "esp_sntp.h"
#include "wifi.h"

void setup_time();
bool get_hour_of_day(uint8_t* hour, uint8_t* minutes);
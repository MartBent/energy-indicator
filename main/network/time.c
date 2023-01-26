#include "time.h"

void setup_time() {
    printf("Setup time...\n");

    setenv("TZ", "	CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    sntp_init();
}

//Gets the hour of the current day in timezone Europe/Amsterdam
bool get_hour_of_day(uint8_t* hour, uint8_t* minutes) {
    setup_time();
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 4;

    //Try to get the network time 4 times, after that stop trying.
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    
    //If no time was found, return false.
    if(retry >= 4) {
        return false;
    }

    time(&now);
    localtime_r(&now, &timeinfo);
    *hour = timeinfo.tm_hour;
    *minutes = timeinfo.tm_min;
    return true; 
}
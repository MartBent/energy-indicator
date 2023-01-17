#include "hardware/buttons.h"
#include "hardware/clock_led.h"
#include "network/wifi.h"
#include "network/http.h"
#include "esp_sleep.h"
#include "memory/solar_data.h"
#include "esp_attr.h"

#include "hardware/display/epd2in9b.h"
#include "hardware/display/epdpaint.h"

#include "hardware/display/fonts.h"

RTC_DATA_ATTR solar_data_cache_t cache = {};

#define COLORED     0
#define UNCOLORED   1

static solar_data_cache_t test_cache = {
    .data_valid = true,
    .data = {
        {
            .data_moment = Today,
            .hour_of_day = 8,
            .watt_hour = 0
        }, 
        {
            .data_moment = Today,
            .hour_of_day = 9,
            .watt_hour = 39
        },
        {
            .data_moment = Today,
            .hour_of_day = 10,
            .watt_hour = 304
        },
        {
            .data_moment = Today,
            .hour_of_day = 11,
            .watt_hour = 427
        },
        {
            .data_moment = Today,
            .hour_of_day = 12,
            .watt_hour = 408
        },
        {
            .data_moment = Today,
            .hour_of_day = 13,
            .watt_hour = 387
        },
        {
            .data_moment = Today,
            .hour_of_day = 14,
            .watt_hour = 356
        },
        {
            .data_moment = Today,
            .hour_of_day = 15,
            .watt_hour = 282
        },
        {
            .data_moment = Today,
            .hour_of_day = 16,
            .watt_hour = 162
        },
        {
            .data_moment = Today,
            .hour_of_day = 17,
            .watt_hour = 36
        }
    }
};


void print_cache() {
    for(int i = 0; i < 10; i++) {
        print_solar_data(cache.data[i]);
    }    
}

void draw_hour_watts(int hour, int watts)
{
    image = (unsigned char*) malloc((128*296) / 8);
    memset(image, 0, 128*296/8);

    Init();

    ClearFrame();
    Clear(UNCOLORED);

    char message[30] = {};
    
    sprintf(message, "%d:00 : %d", hour, watts);

    DrawStringAt(0, 0, message, &Font24, COLORED);

    SetPartialWindowBlack(image, 0, 0, EPD_WIDTH, EPD_HEIGHT);

    DisplayFrameRam();
    Sleep();
    free(image);
}

bool handle_timer_wakeup(settings_t* settings)
{
    setup_wifi();
    
    bool ok = setup_sta(settings);
    if(!ok) {
        printf("Error connecting to wifi\n");
        return false;
    }

    bool result = false;
    char* response = malloc(1000);

    //Re-enable wifi
    start_and_connect();

    int length = 0;
    if(http_get_request("https://api.forecast.solar/estimate/watt_hours_period/52.011509/6.701236/37/0/5.67", response, &length)) {
        printf("%.*s\n", length, response);

        char* line;
        uint16_t counter = 0;

        while ((line = strtok_r(response, "\n", &response))) {
            //Line is valid if it is shorter then 30 chars
            if(strlen(line) <= 30) {

                //We're gonna assume that the API returns 10 hours of each day, so 8 AM + counter. MOD 10 since 2 days of 10 hours are returned.
                char* date_time = strtok_r(line, ";", &line); 
                int watts = atoi(strtok_r(line, ";", &line));
                strtok_r(date_time, " ", &date_time);
                int time = 8 + (counter % 10);
               
                solar_data_t line_data = {
                    .data_moment = counter < 10 ? Today : Tormorrow,
                    .hour_of_day = time,
                    .watt_hour = watts
                };

                if(counter <= 19) {
                    cache.data[counter] = line_data;
                }

                counter++;
            }
            if(counter == 10) {
                cache.data_valid = true;
                result = true;
                print_cache();
                break;
            }
        }      
    }
    else {
        printf("The GET request resulted in error: %d\n", length);
    }
    disable_wifi();
    return result;
}

void handle_sensor_wakeup() {
    setup_clock_led();
    printf("Handling sensor wakeup...\n");

    solar_data_cache_t* cache_ptr = &cache;
    if(!cache.data_valid) {
        printf("No valid data in memory, using test data...\n");
        cache_ptr = &test_cache;
    }

    solar_data_t max_data = {
        .data_moment = Today,
        .hour_of_day = 0,
        .watt_hour = 0
    };

    if(cache_ptr->data_valid) {
        //Get best solar hour of today
        for(int i = 0; i < 10; i++) {
            solar_data_t current_data = cache_ptr->data[i];
            if(current_data.watt_hour > max_data.watt_hour) {
                max_data = current_data;
            }
        }

        display_hour(max_data.hour_of_day);
        draw_hour_watts(max_data.hour_of_day, max_data.watt_hour);
        
        //vTaskDelay(3000 / portTICK_PERIOD_MS);

        disable_clock_led();

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } else {
        printf("Data cache is empty\n");
        display_hour(0);
    }
}

void handle_reset_wakeup() {
    printf("Handling reset wakeup...\n");
    vTaskDelay(300 / portTICK_PERIOD_MS);
    uint32_t counter = 0;
    while(gpio_get_level(RESET_BUTTON) == 1) {
        vTaskDelay(50 / portTICK_RATE_MS);
        ++counter;
        if(counter > 100) {
            erase_settings();
            esp_restart();
        }
    }
}

void deep_sleep(uint32_t sleep_interval_seconds) {
    //Set sleep timer and go into sleep
    printf("Going into sleep...\n");
    esp_sleep_enable_timer_wakeup(sleep_interval_seconds * 1000000);
    
    uint64_t pin_mask = (uint64_t)1 << RESET_BUTTON;
    
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(pin_mask, ESP_EXT1_WAKEUP_ANY_HIGH));
    esp_deep_sleep_start();
}

void app_main(void)
{
    setup_flash();
    setup_settings();
    
    //Display
    Init();
    ClearFrame();
    Clear(UNCOLORED);

    settings_t settings;
    bool settings_found = retrieve_settings(&settings);

    if(settings_found) {
        printf("Settings found, connecting...\n"); 
        bool data_retrieved = false;

        //Depending on how the device was started, run certain tasks
        esp_sleep_source_t wakeup_cause = esp_sleep_get_wakeup_cause();

        if(wakeup_cause == ESP_SLEEP_WAKEUP_EXT1) {
            //Check pin num
            uint64_t pin_mask = esp_sleep_get_ext1_wakeup_status();

            bool did_sensor_wakeup = (pin_mask >> SENSOR_INT) == 1;
            bool did_button_wakeup = (pin_mask >> RESET_BUTTON) == 1;

            if(did_button_wakeup) {
                //handle_reset_wakeup();
                handle_sensor_wakeup();

            } 
            else if(did_sensor_wakeup) {
                handle_sensor_wakeup();
            }
        }
        else if (wakeup_cause == ESP_SLEEP_WAKEUP_TIMER) {
            data_retrieved = handle_timer_wakeup(&settings);
        }
        else {
            //The device should always retrieve solar data from the internet on startup.
            printf("Unknown wakeup cause / Inital startup\n");
            data_retrieved = handle_timer_wakeup(&settings);
        }
        //Sleep for only an hour if no data is retrieved. Otherswise for an whole day.
        deep_sleep(data_retrieved ? 60*60*24 : 60*60);
    } else {
        printf("Settings not found, setting up AP...\n");
        setup_access_point();
    }
    while(1) {}
}  

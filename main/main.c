#include "esp_sleep.h"

#include "network/wifi.h"
#include "network/http.h"
#include "network/time.h"

#include "memory/solar_data.h"

#include "hardware/clock_led.h"
#include "hardware/display/epd2in9b.h"
#include "hardware/display/epdpaint.h"
#include "hardware/display/fonts.h"
#include "hardware/battery.h"

//The RTC_DATA_ATTR is required since RAM memory is erased during deep_sleep, but RTC memory is kept on, so store the weather data there.
RTC_DATA_ATTR solar_data_cache_t cache = {};

//Gets the hour from memory with the most solar collection
solar_data_t* get_max_hour_data(bool use_test_data) {

    solar_data_t* result;
    
    solar_data_cache_t* cache_ptr = &cache;
    //Code for testing: if data is not received still display some testing data
    if(!cache.data_valid && use_test_data) {
        printf("No valid data in memory, using test data...\n");
        cache_ptr = &test_cache;
    }

    if(cache_ptr->data_valid) {
        result = &cache_ptr->data[0];
        for(int i = 0; i < 10; i++) {
            if(cache_ptr->data[i].watt_hour > result->watt_hour) {
                result = &cache_ptr->data[i];
            }
        }
        return result;
    } else {
        return NULL;
    }
}

//This handles the timer and startup wakeup. It retrieve solar data from forecast.solar and save it to RTC memory. It also refreshes the data shown on the ePaper display. 
bool handle_timer_wakeup(settings_t* settings)
{
    bool result = false;

    setup_wifi();
    
    bool ok = setup_sta(settings);
    if(!ok) {
        printf("Error connecting to wifi\n");
        result = false;
    }

    //If WiFi is connected, make GET request
    if(ok) {
        char* response = malloc(1000);
        
        int length = 0;
        if(http_get_request("https://api.forecast.solar/estimate/watt_hours_period/52.011509/6.701236/37/0/5.67", response, &length)) {
            printf("%.*s\n", length, response);

            char* line;
            uint16_t counter = 0;
            
            //This loop decoded the retrieve data, this data will be in CSV format so it needs to be split into group seperated by the ; symbol.
            while ((line = strtok_r(response, "\n", &response))) {
                //Line is valid if it is shorter then 30 chars
                if(strlen(line) <= 30) {

                    //We're gonna assume that the API returns 10 hours of each day, so 8 AM + counter. MOD 10 since 2 days of 10 hours are returned.
                    //Slit each line into Time : WattHour
                    char* date_time = strtok_r(line, ";", &line); 
                    int watts = atoi(strtok_r(line, ";", &line));
                    strtok_r(date_time, " ", &date_time);
                    int time = 8 + (counter % 10);
                    solar_data_t line_data = {
                        .data_moment = counter < 10 ? Today : Tormorrow,
                        .hour_of_day = time,
                        .watt_hour = watts
                    };

                    //Add the line data to the cache
                    if(counter <= 19) {
                        cache.data[counter] = line_data;
                    }

                    counter++;
                }

                //If all of todays data is retrieve, print it and mark it as valid. break out of loop since we dont need tomorrows data.
                if(counter == 10) {
                    cache.data_valid = true;
                    result = true;
                    print_cache(cache);
                    break;
                }
            }      
        }
        else {
            printf("The GET request resulted in error: %d\n", length);
        }
    }

    //Get the retrieve data from memory
    solar_data_t* max_data = get_max_hour_data(true);

    if(max_data != NULL) {

        //Get time and calculate delta

        uint8_t hour = 0, minutes = 0;

        //Get current time from internet
        bool ok = get_hour_of_day(&hour, &minutes);

        //If the time was received, calculate the differnce between this and the best hour of the solar data.
        if(ok) {
            
            uint8_t delta_hour = 0, delta_minutes = 0;
            
            if(max_data->hour_of_day > hour) {
                delta_hour = max_data->hour_of_day - hour - 1;
            }

            delta_minutes = 60 - minutes;
            if(delta_minutes == 60) {
                delta_hour += 1;
                delta_minutes = 0;
            }
            
            //Draw the time to the display
            display_draw_time(delta_hour, delta_minutes);
            printf("Delta time: %d:%d...\n", delta_hour, delta_minutes);

        } else {
            printf("Error receiving time...\n");
        }

    }

    //Disable the wifi
    disable_wifi();

    return result;
}

//This handles the reset wakeup, if the reset button is held down for 5 seconds after waking up the settings are reset and a LED animation is played. The device will restart after.
void handle_reset_wakeup() {
    vTaskDelay(300 / portTICK_PERIOD_MS);
    uint32_t counter = 0;
    while(gpio_get_level(RESET_BUTTON) == 1) {
        vTaskDelay(50 / portTICK_RATE_MS);
        ++counter;
        if(counter > 100) {
            setup_clock_led();
            disable_clock_led();
            clock_led_animate();
            vTaskDelay(500 / portTICK_RATE_MS);
            disable_clock_led();
            battery_disable_warning_led();
            battery_disable_ok_led();
            network_disable_led();
            vTaskDelay(500 / portTICK_RATE_MS);
            erase_settings();
            esp_restart();
        }
    }
}

//This function handles the sensor wakeup, it will retrieve data from memory and display it on the LED strip
//It will also retrieve the current time and use it to calculate which time to show on the ePaper display.
void handle_sensor_wakeup() {

    //Setup necesarry hardware
    setup_clock_led();
    battery_init_led();
    network_init_led();

    printf("Handling sensor wakeup...\n");

    //Retrieve this data from memory.
    solar_data_t* max_data_ptr = get_max_hour_data(true);

    //If the data exists in memory, show it on the LED strip
    if(max_data_ptr != NULL) {
        
        //Hard coded for now, needs to be retrieved from internet in the future
        const uint8_t community_performance = 3;

        //Display the data on the LED strip
        clock_led_display_data(max_data_ptr->hour_of_day % 12, community_performance);
        
        //Get the battery voltage
        uint32_t voltage = battery_read_voltage();
        printf("Battery voltage: %d\n", voltage);
        
        //Turn on the warning LED if battery is low
        if(voltage < 1500) {
            battery_enable_warning_led();
        } else if(voltage > 2100) {
            //Turn on the battery OK led if it is full
            battery_enable_ok_led();
        }
        
        //If no valid data is in memory, turn on the network warning LED, since the device might not have a working internet connection.
        if(!cache.data_valid) {
            network_enable_led();
        }
        //Keep everything turned on for 15 seconds, if the reset button is pressed during this period, handle it the same way it would on wake up.
        vTaskDelay(15000 / portTICK_PERIOD_MS);
        for(int i = 0; i < 150; i++) {
            handle_reset_wakeup();
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        //Disable all the LEDs
        disable_clock_led();
        battery_disable_warning_led();
        battery_disable_ok_led();
        network_disable_led();

        //Give the LED strip time to turn off
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } else {
        printf("Data cache is empty\n");

        //Show network LED when no data is avaiable.
        network_enable_led();
        disable_clock_led();

        //Keep LEDS turned on for 5 seconds 
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        network_disable_led();
    }
}

//This function puts the ESP32 into deep sleep. It configures the RESET_BUTTON and SENSOR_INT as wakeup sources. It is also able to wakeup from the timer.
void deep_sleep(uint32_t sleep_interval_seconds) {
    //Set sleep timer and go into sleep
    printf("Going into sleep...\n");
    //Set the timer timeout in seconds.
    esp_sleep_enable_timer_wakeup(sleep_interval_seconds * 1000000);
    
    //Define the wakeup sources 
    uint64_t pin_mask = (uint64_t)1 << RESET_BUTTON;
    pin_mask |= (uint64_t)1 << SENSOR_INT;

    //Set the wakeup sources
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(pin_mask, ESP_EXT1_WAKEUP_ANY_HIGH));

    //Go to sleep, this will result in the device restarting from app_main(void).
    esp_deep_sleep_start();
}

void app_main(void)
{
    //Setup flash and partitions to check if settings are set and retrieve them.
    setup_flash();
    setup_settings();

    settings_t settings;

    //Retrieve settings and put them in the settings_t struct if they exists in memory.
    bool settings_found = retrieve_settings(&settings);

    //If settings are found, determine how the device woke up, and handle this wake up
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
                printf("Handling reset wakeup...\n");
                handle_reset_wakeup();
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
    } 
    else //If the device has no settings in memory, turn the device into a Wi-Fi access point so the user can connect to it. 
    {
        //In case the device was reset while displaying data on the LED strip, it needs to be fully cleared since it messes with the Wi-Fi driver.
        setup_clock_led();
        disable_clock_led();

        //Give the LED strip one seconds to turn off
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        //Setup the Wi-Fi access point.
        printf("Settings not found, setting up AP...\n");
        setup_access_point();
    }
    while(1) {}
}  

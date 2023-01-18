#ifndef SOLAR_C
#define SOLAR_C

#include "solar_data.h"

void print_solar_data(solar_data_t data) {
    const char* prefix = data.data_moment == Today ? "Today:" : "Tomorrow: ";
    printf("%s %d | %d\n", prefix, data.hour_of_day, data.watt_hour);
}

void print_cache(solar_data_cache_t cache) {
    if(cache.data_valid) {
        for(int i = 0; i < 10; i++) {
            print_solar_data(cache.data[i]);
        }   
    } 
}

#endif
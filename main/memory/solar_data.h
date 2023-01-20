#ifndef SOLAR_H
#define SOLAR_H

#include <stdbool.h>
#include <stdio.h>

#include "esp_attr.h"

typedef enum {
    Today,
    Tormorrow
} data_moment_t;

typedef struct {
    data_moment_t data_moment;
    int hour_of_day;
    int watt_hour;
} solar_data_t;

typedef struct {
    solar_data_t data[20];
    bool data_valid;
} solar_data_cache_t;


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
            .watt_hour = 2822
        },
        {
            .data_moment = Today,
            .hour_of_day = 16,
            .watt_hour = 162
        },
        {
            .data_moment = Today,
            .hour_of_day = 17,
            .watt_hour = 3111
        }
    }
};

void print_solar_data(solar_data_t data);
void print_cache(solar_data_cache_t cache);

#endif
#ifndef SOLAR_H
#define SOLAR_H

#include <stdbool.h>

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

void print_solar_data(solar_data_t data);

#endif
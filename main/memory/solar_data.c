#ifndef SOLAR_C
#define SOLAR_C

#include "solar_data.h"
#include <stdio.h>

void print_solar_data(solar_data_t data) {
    const char* prefix = data.data_moment == Today ? "Today:" : "Tomorrow: ";
    printf("%s %d | %d\n", prefix, data.hour_of_day, data.watt_hour);
}

#endif
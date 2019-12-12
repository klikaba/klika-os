#ifndef __CLOCK_H
#define __CLOCK_H

#include <kernel.h>

#define RTC_EPOCH_BASE_YEAR 1970
#define RTC_EPOCH_JULIAN_DAY 2440588
#define RTC_SECONDS_DAY 86400

// Struct to hold CMOS data
typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t century;
} CmosData;

// Struct to represent Datetime object
typedef struct
{
    uint32_t second;
    uint32_t minute;
    uint32_t hour;
    uint32_t day;
    uint32_t month;
    uint32_t year;
} DateTime;

// Public getter and setter functions for hardware datetime
uint32_t get_hw_datetime();
void set_hw_datetime(uint32_t timestamp);

// Returns 1 if two DateTime objects are equal
int datetime_equals(CmosData *a, CmosData *b);

// Conversion methods
uint32_t datetime_to_timestamp(DateTime *dt);
void timestamp_to_datetime(uint32_t timestamp, DateTime *dt);

// struct DateTime clock_get_time();
void init_kernel_clock();

#endif
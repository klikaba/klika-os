#ifndef __TIME_H
#define __TIME_H

#include <stdint.h>

/**
 * Original time.h library defines these constants:
 * --> NULL:
 *      This macro is the value of a null pointer constant.
 *      Already defined in ./types.h
 * --> CLOCKS_PER_SEC:
 *      This macro represents the number of processor clocks per second.
 * 
 * Original time.h library defines these data types:
 * --> size_t:
 *      This is the unsigned integral type and is the result of the sizeof keyword.
 *      Already defined in ./types.h
 * * --> clock_t:
 *      This is a type suitable for storing the processor time.
 * * --> time_t:
 *      This is a type suitable for storing the calendar time.
 * * --> struct tm:
 *      This is a structure used to hold the time and date.
 * */

/**
 * This macro represents the number of processor clocks per second.
 * */
#define CLOCKS_PER_SEC 1000000

/**
 * This is a type suitable for storing the processor time.
 * */
typedef uint32_t clock_t;

/**
 * This is a type suitable for storing the calendar time.
 * */
typedef uint64_t time_t;

/**
 * This is a structure used to hold the time and date.
 * */
typedef struct tm
{
    int tm_sec;   // seconds,  range 0 to 59
    int tm_min;   // minutes, range 0 to 59
    int tm_hour;  // hours, range 0 to 23
    int tm_mday;  // day of the month, range 1 to 31
    int tm_mon;   // month, range 0 to 11
    int tm_year;  // The number of years since 1900
    int tm_wday;  // day of the week, range 0 to 6
    int tm_yday;  // day in the year, range 0 to 365
    int tm_isdst; // daylight saving time
} tm;

/**
 * Get the current time (number of seconds from the epoch) from the system clock. 
 * Stores that value in timer. If timer is null, the value is not stored, but it is still returned by the function.
 * */
extern time_t time(time_t *timer);

/**
 * Gets the number of clock ticks since the process started.
 * */
extern clock_t clock(void);

/**
 * Primitive implementation of sleep function. 
 * This functions execution lasts for provided number of miliseconds.
 * */
extern void sleep(clock_t clock_ticks);

/**
 * Convert tm to a time_t time value. 
 * Checks the members of the tm structure passed as parameter ptm adjusting the values if the ones provided are not in the possible range 
 * or they are incomplete or mistaken and then translates that structure to a time_t value that is returned. 
 * The original values of tm_wday and tm_yday members of ptm are ignored and filled with the correspondent ones to the calculated date. 
 * The range of tm_mday is not checked until tm_mon and tm_year are determined. On error, a -1 value is returned.
 * */
extern time_t mktime(struct tm *tm);
extern void mkdatetime(time_t timestamp, struct tm *tm);

/**
 * Formats 'struct tm' into a date/time string.
 * If the resulting C string fits in less than size characters (which includes the terminating null-character), 
 * the total number of characters copied to str (not including the terminating null-character) is returned otherwise, it returns zero.
 * 
 * Currently it only supports format -> 'dd/MM/yyyy ss:mm:hh'
 * Complete signature should be -> 'size_t strftime(char *buffer, size_t maxSize, const char *format, const struct tm *tm);'
 * */
extern size_t strftime(char *buffer, size_t maxSize, const struct tm *tm);

/**
 * Scan values from buf string into tptr struct. 
 * On success it returns pointer to the character following the last character parsed. 
 * Otherwise it returns null.
 * 
 * Currently it only supports format -> 'ss:mm:hh dd/MM/yyyy'
 * Complete signature should be -> 'char *strptime(const char *buf, const char *format, struct tm *tm);'
 * */
// extern char *strptime(const char *buf, struct tm *tm);

/**
 * Returns time2 minus time1 to give the difference in seconds between the two times.
 * */
extern time_t difftime(time_t time1, time_t time2);

#endif
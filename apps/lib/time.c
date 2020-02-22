#include <stdlib.h>
#include <time.h>
#include <syscalls.h>
#include <string.h>
#include <malloc.h>

#define RTC_EPOCH_BASE_YEAR 1970
#define RTC_EPOCH_JULIAN_DAY 2440588
#define RTC_SECONDS_DAY 86400

unsigned long _get_rtc_timestamp();
unsigned long _get_rtc_clock_ticks();

// Public interface
time_t time(time_t *timer)
{
    time_t timestamp = _get_rtc_timestamp();

    if (timer != NULL)
    {
        *timer = timestamp;
    }

    return timestamp;
}

clock_t clock(void)
{
    clock_t clock_ticks = _get_rtc_clock_ticks();
    return clock_ticks;
}

void sleep(clock_t clock_ticks)
{
    clock_t initial_clock = clock();

    while (clock() - initial_clock < clock_ticks)
        ;
}

time_t mktime(struct tm *tm)
{
    int days, year, month;

    if (tm == NULL)
    {
        return -1;
    }

    month = tm->tm_mon + 1;
    year = tm->tm_year + RTC_EPOCH_BASE_YEAR;

    // Reference: Fliegel, H. F. and van Flandern, T. C. (1968).
    // Communications of the ACM, Vol. 11, No. 10 (October, 1968).
    days = tm->tm_mday - 32075 - RTC_EPOCH_JULIAN_DAY +
           1461 * (year + 4800 + (month - 14) / 12) / 4 +
           367 * (month - 2 - 12 * ((month - 14) / 12)) / 12 -
           3 * ((year + 4900 + (month - 14) / 12) / 100) / 4;

    return days * RTC_SECONDS_DAY + tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;
}

void mkdatetime(time_t timestamp, struct tm *tm)
{
    int year, month, day, l, n;

    // Reference: Fliegel, H. F. and van Flandern, T. C. (1968).
    // Communications of the ACM, Vol. 11, No. 10 (October, 1968).
    l = timestamp / RTC_SECONDS_DAY + 68569 + RTC_EPOCH_JULIAN_DAY;
    n = 4 * l / 146097;
    l = l - (146097 * n + 3) / 4;
    year = 4000 * (l + 1) / 1461001;
    l = l - 1461 * year / 4 + 31;
    month = 80 * l / 2447;
    day = l - 2447 * month / 80;
    l = month / 11;
    month = month + 2 - 12 * l;
    year = 100 * (n - 49) + year + l;

    tm->tm_mday = day;
    tm->tm_mon = month;
    tm->tm_year = year;

    timestamp = timestamp % RTC_SECONDS_DAY;
    tm->tm_hour = timestamp / 3600;

    timestamp = timestamp % 3600;
    tm->tm_min = timestamp / 60;
    tm->tm_sec = timestamp % 60;
}

size_t strftime(char *buffer, size_t maxSize, const struct tm *tm)
{
    const size_t formatLength = 21;

    if (maxSize < formatLength)
    {
        return 0;
    }

    sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d",
            tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_hour, tm->tm_min, tm->tm_sec);

    return formatLength - 1;
}

time_t difftime(time_t time1, time_t time2)
{
    return time2 - time1;
}

// Helper methods
unsigned long _get_rtc_timestamp()
{
    return syscall(SYSCall_get_clock);
}

unsigned long _get_rtc_clock_ticks()
{
    return syscall(SYSCall_get_clock_ticks);
}
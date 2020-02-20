#include <clock.h>
#include <x86.h>
#include <pic.h>
#include <isr.h>

#define CMOS_ADDRESS_PORT 0x70
#define CMOS_DATA_PORT 0x71

#define CMOS_REGISTER_SECONDS 0x00
#define CMOS_REGISTER_MINUTES 0x02
#define CMOS_REGISTER_HOURS 0x04
#define CMOS_REGISTER_WEEKDAY 0x06
#define CMOS_REGISTER_DAY_OF_MONTH 0x07
#define CMOS_REGISTER_MONTH 0x08
#define CMOS_REGISTER_YEAR 0x09
#define CMOS_REGISTER_CENTURY 0x32
#define CMOS_REGISTER_STATUS_A 0x0A
#define CMOS_REGISTER_STATUS_B 0x0B
#define CMOS_REGISTER_STATUS_C 0x0C

void NMI_enable()
{
    outp(CMOS_ADDRESS_PORT, CMOS_REGISTER_STATUS_B);
    outp(CMOS_DATA_PORT, inp(CMOS_DATA_PORT) | 0x40);
}

void NMI_disable()
{
    outp(CMOS_ADDRESS_PORT, CMOS_REGISTER_STATUS_B);
    outp(CMOS_DATA_PORT, inp(CMOS_DATA_PORT) & 0xBF);
}

// Utility methods
int datetime_equals(CmosData *a, CmosData *b)
{
    return a->second == b->second && a->minute == b->minute && a->hour == b->hour &&
           a->day == b->day && a->month == b->month && a->year == b->year &&
           a->century == b->century;
}

// Conversion
uint32_t bcd_to_int(uint8_t bcd)
{
    uint32_t numl;
    uint32_t numh;

    numl = bcd & 0x0f;
    numh = (bcd & 0xf0) >> 4;

    return numh * 10 + numl;
}

uint8_t int_to_bcd(uint32_t number)
{
    uint8_t low;
    uint8_t high;

    if (number > 99)
        return 0;

    high = number / 10;
    low = number % 10;

    return (high << 4) | low;
}

uint32_t _cmos_to_timestamp(const CmosData *cmos)
{
    DateTime t;
    t.year = bcd_to_int(cmos->century) * 100 + bcd_to_int(cmos->year) - RTC_EPOCH_BASE_YEAR;
    t.month = bcd_to_int(cmos->month) - 1;
    t.day = bcd_to_int(cmos->day);
    t.hour = bcd_to_int(cmos->hour);
    t.minute = bcd_to_int(cmos->minute);
    t.second = bcd_to_int(cmos->second);

    return datetime_to_timestamp(&t);
}

void _timestamp_to_cmos(uint32_t timestamp, CmosData *cmos)
{
    int wholeYear;
    DateTime t;

    DEBUG("Timestamp: %i", timestamp);

    timestamp_to_datetime(timestamp, &t);
    wholeYear = t.year + RTC_EPOCH_BASE_YEAR;

    cmos->century = int_to_bcd(wholeYear / 100);
    cmos->year = int_to_bcd(wholeYear % 100);
    cmos->month = int_to_bcd(t.month + 1);
    cmos->day = int_to_bcd(t.day);
    cmos->hour = int_to_bcd(t.hour);
    cmos->minute = int_to_bcd(t.minute);
    cmos->second = int_to_bcd(t.second);
}

// Accessing CMOS register values
uint8_t _read_cmos_register(uint8_t registerAddress)
{
    uint32_t maxTries = 10000;

    // Wait until bit 7 of Status Register A (indicating whether or not an update is in
    // progress) is clear if we are reading one of the clock data registers...
    if (registerAddress < CMOS_REGISTER_STATUS_A)
    {
        outp(CMOS_ADDRESS_PORT, CMOS_REGISTER_STATUS_A);
        while ((inp(CMOS_DATA_PORT) & 0x80) && --maxTries)
            ;
    }

    // then read the value.
    outp(CMOS_ADDRESS_PORT, registerAddress);
    return inp(CMOS_DATA_PORT);
}

void _write_cmos_register(uint8_t registerAddress, uint8_t data)
{
    outp(CMOS_ADDRESS_PORT, registerAddress);
    outp(CMOS_DATA_PORT, data);
}

void _set_24_hour_mode()
{
    uint8_t status_b = _read_cmos_register(CMOS_REGISTER_STATUS_B);
    status_b |= 0x02;
    _write_cmos_register(CMOS_REGISTER_STATUS_B, status_b);
}

// Accessing CMOS values
void _read_cmos_clock(CmosData *dt)
{
    _set_24_hour_mode();

    dt->century = _read_cmos_register(CMOS_REGISTER_CENTURY);
    dt->year = _read_cmos_register(CMOS_REGISTER_YEAR);
    dt->month = _read_cmos_register(CMOS_REGISTER_MONTH);
    dt->day = _read_cmos_register(CMOS_REGISTER_DAY_OF_MONTH);
    dt->hour = _read_cmos_register(CMOS_REGISTER_HOURS);
    dt->minute = _read_cmos_register(CMOS_REGISTER_MINUTES);
    dt->second = _read_cmos_register(CMOS_REGISTER_SECONDS);
}

void _write_cmos_clock(CmosData *dt)
{
    _set_24_hour_mode();

    _write_cmos_register(CMOS_REGISTER_CENTURY, dt->century);
    _write_cmos_register(CMOS_REGISTER_YEAR, dt->year);
    _write_cmos_register(CMOS_REGISTER_MONTH, dt->month);
    _write_cmos_register(CMOS_REGISTER_DAY_OF_MONTH, dt->day);
    _write_cmos_register(CMOS_REGISTER_HOURS, dt->hour);
    _write_cmos_register(CMOS_REGISTER_MINUTES, dt->minute);
    _write_cmos_register(CMOS_REGISTER_SECONDS, dt->second);
}

// Public ----------------------------------->
uint32_t datetime_to_timestamp(DateTime *dt)
{
    int days, year, month;

    month = dt->month + 1;
    year = dt->year + RTC_EPOCH_BASE_YEAR;

    // Reference: Fliegel, H. F. and van Flandern, T. C. (1968).
    // Communications of the ACM, Vol. 11, No. 10 (October, 1968).
    days = dt->day - 32075 - RTC_EPOCH_JULIAN_DAY +
           1461 * (year + 4800 + (month - 14) / 12) / 4 +
           367 * (month - 2 - 12 * ((month - 14) / 12)) / 12 -
           3 * ((year + 4900 + (month - 14) / 12) / 100) / 4;

    return days * RTC_SECONDS_DAY + dt->hour * 3600 + dt->minute * 60 + dt->second;
}

void timestamp_to_datetime(uint32_t timestamp, DateTime *dt)
{
    uint32_t year, month, day, l, n;

    // Reference: Fliegel, H. F. and van Flandern, T. C. (1968).
    // Communications of the ACM, Vol. 11, No. 10 (October, 1968).
    l = timestamp / 86400 + 68569 + RTC_EPOCH_JULIAN_DAY;
    n = 4 * l / 146097;
    l = l - (146097 * n + 3) / 4;
    year = 4000 * (l + 1) / 1461001;
    l = l - 1461 * year / 4 + 31;
    month = 80 * l / 2447;
    day = l - 2447 * month / 80;
    l = month / 11;
    month = month + 2 - 12 * l;
    year = 100 * (n - 49) + year + l;

    dt->day = day;
    dt->month = month - 1;
    dt->year = year - RTC_EPOCH_BASE_YEAR;

    timestamp = timestamp % RTC_SECONDS_DAY;
    dt->hour = timestamp / 3600;

    timestamp = timestamp % 3600;
    dt->minute = timestamp / 60;
    dt->second = timestamp % 60;
}

uint32_t get_hw_datetime()
{
    uint32_t maxTries = 100;
    CmosData dt, previousDT;

    // Clock is read until two consecutive reads are equal.  This will prevent
    // problems that would occur if the clock is read during an update (e.g. if we read the hour
    // at 8:59:59, the clock gets changed, and then we read the minute and second, we would
    // be off by a whole hour)
    do
    {
        _read_cmos_clock(&dt);
        _read_cmos_clock(&previousDT);
    } while (!datetime_equals(&dt, &previousDT) && --maxTries);

    // dt should now contain the correct values
    return _cmos_to_timestamp(&dt);
}

void set_hw_datetime(uint32_t timestamp)
{
    CmosData dt;
    _timestamp_to_cmos(timestamp, &dt);
    _write_cmos_clock(&dt);
}

static void rtc_tick_callback(isr_ctx_t *ctx __UNUSED__)
{
    DEBUG("Something");
    outp(CMOS_ADDRESS_PORT, CMOS_REGISTER_STATUS_C);
    uint8_t value = inp(CMOS_DATA_PORT);

    DEBUG("RTC tick callback: %i", value);

    pic_acknowledge(PIC_IRQ8);
}

// Init of clock module
void init_kernel_clock()
{
    NMI_disable();

    // Enabling IRQ8 with frequency of 1024Hz
    outp(CMOS_ADDRESS_PORT, CMOS_REGISTER_STATUS_B | 0x80);
    uint8_t prev = inp(CMOS_DATA_PORT);
    
    outp(CMOS_ADDRESS_PORT, CMOS_REGISTER_STATUS_B | 0x80);
    outp(CMOS_ADDRESS_PORT, prev | 0x40);

    // Register interrupt handler
    register_interrupt_handler(ISR_IRQ8, rtc_tick_callback);
    irq_enable(PIC_IRQ8);

    NMI_enable();
};
#ifndef PICO_CLOCK_RTC_H
#define PICO_CLOCK_RTC_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "constants.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MONDAY = 1,
    TUESDAY = 2,
    WEDNESDAY = 3,
    THURSDAY = 4,
    FRIDAY = 5,
    SATURDAY = 6,
    SUNDAY = 7,
} WeekDay;

typedef struct DateTime {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    WeekDay weekDay;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} DateTime;

bool rtc_dateTimeEquals(const DateTime *a, const DateTime *b);

const char *rtc_weekdayToString(const WeekDay weekDay);

const char *rtc_monthToString(const uint8_t month);

void rtc_init();

bool rtc_isRunning();

bool rtc_isErrored();

bool rtc_isBatteryEnabled();

uint32_t rtc_getBaudRate();

uint8_t rtc_getSeconds();

uint8_t rtc_getMinutes();

uint8_t rtc_getHours();

WeekDay rtc_getWeekday();

uint8_t rtc_getDate();

uint8_t rtc_getMonth();

uint8_t rtc_getYear();

void rtc_getDateTime(DateTime *const result);

void rtc_setIsRunning(const bool isRunning);

void rtc_setIsErrored(const bool isErrored);

void rtc_setIsBatteryEnabled(const bool isBatteryEnabled);

void rtc_setBaudRate(const uint32_t baudrate);

void rtc_setSeconds(const uint8_t seconds);

void rtc_setMinutes(const uint8_t minutes);

void rtc_setHours(const uint8_t hours);

void rtc_setWeekday(const WeekDay weekday);

void rtc_setDate(const uint8_t date);

void rtc_setMonth(const uint8_t month);

void rtc_setYear(const uint8_t year);

void rtc_setDateTime(const DateTime *const dateTime);

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_RTC_H

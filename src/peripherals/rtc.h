#ifndef PICO_CLOCK_RTC_H
#define PICO_CLOCK_RTC_H

#include "../types.h"
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void rtc_init();

extern void rtc_setDateChangedCallback(const DateChangedCallback callback);

extern void rtc_setTimeChangedCallback(const TimeChangedCallback callback);

extern void rtc_setDateTimeChangedCallback(const DateTimeChangedCallback callback);

extern bool rtc_isRunning();

extern bool rtc_isErrored();

extern bool rtc_isBatteryEnabled();

extern uint32_t rtc_getBaudRate();

extern uint8_t rtc_getSeconds();

extern uint8_t rtc_getMinutes();

extern uint8_t rtc_getHours();

extern WeekDay rtc_getWeekday();

extern uint8_t rtc_getDay();

extern uint8_t rtc_getMonth();

extern uint8_t rtc_getYear();

extern void rtc_getDate(Date *const result);

extern void rtc_getTime(Time *const result);

extern void rtc_getDateTime(DateTime *const result);

extern void rtc_setIsRunning(const bool isRunning);

extern void rtc_setIsErrored(const bool isErrored);

extern void rtc_setIsBatteryEnabled(const bool isBatteryEnabled);

extern void rtc_setBaudRate(const uint32_t baudrate);

extern void rtc_setSeconds(const uint8_t seconds);

extern void rtc_setMinutes(const uint8_t minutes);

extern void rtc_setHours(const uint8_t hours);

extern void rtc_setWeekday(const WeekDay weekday);

extern void rtc_setDay(const uint8_t day);

extern void rtc_setMonth(const uint8_t month);

extern void rtc_setYear(const uint8_t year);

extern void rtc_setDate(const Date *const date);

extern void rtc_setTime(const Time *const time);

extern void rtc_setDateTime(const DateTime *const dateTime);

extern void rtc_loop();

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_RTC_H

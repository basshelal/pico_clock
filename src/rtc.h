#ifndef PICO_CLOCK_RTC_H
#define PICO_CLOCK_RTC_H

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

bool dateTimeEquals(const DateTime *const a, const DateTime *const b);

const char *weekdayToString(const WeekDay weekDay);

const char *monthToString(const uint8_t month);

void rtcInit();

bool rtcIsRunning();

bool rtcIsWritable();

uint32_t rtcGetBaudRate();

uint8_t rtcGetSeconds();

uint8_t rtcGetMinutes();

uint8_t rtcGetHours();

uint8_t rtcGetDate();

uint8_t rtcGetMonth();

WeekDay rtcGetWeekday();

uint8_t rtcGetYear();

void rtcGetDateTime(DateTime *const result);

void rtcSetRunning(const bool running);

void rtcSetWritable(const bool writable);

void rtcSetBaudRate(const uint32_t baudrate);

void rtcSetSeconds(const uint8_t seconds);

void rtcSetMinutes(const uint8_t minutes);

void rtcSetHours(const uint8_t hours);

void rtcSetDate(const uint8_t date);

void rtcSetMonth(const uint8_t month);

void rtcSetWeekday(const WeekDay weekday);

void rtcSetYear(const uint8_t year);

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_RTC_H

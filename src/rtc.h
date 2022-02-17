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

bool dateTimeEquals(DateTime a, DateTime b);

const char *weekdayToString(WeekDay weekDay);

const char *monthToString(uint8_t month);

void rtcInit();

bool rtcIsRunning();

bool rtcIsWritable();

uint rtcGetBaudRate();

uint8_t rtcGetSeconds();

uint8_t rtcGetMinutes();

uint8_t rtcGetHours();

uint8_t rtcGetDate();

uint8_t rtcGetMonth();

WeekDay rtcGetWeekday();

uint8_t rtcGetYear();

DateTime rtcGetDateTime();

void rtcSetRunning(bool running);

void rtcSetWritable(bool writable);

void rtcSetBaudRate(uint baudrate);

void rtcSetSeconds(uint8_t seconds);

void rtcSetMinutes(uint8_t minutes);

void rtcSetHours(uint8_t hours);

void rtcSetDate(uint8_t date);

void rtcSetMonth(uint8_t month);

void rtcSetWeekday(WeekDay weekday);

void rtcSetYear(uint8_t year);

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_RTC_H

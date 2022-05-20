#ifndef PICO_CLOCK_TYPES_H
#define PICO_CLOCK_TYPES_H

#include "pico/stdlib.h"

typedef struct Rectangle {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} Rectangle;

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef enum {
    MONDAY = 1,
    TUESDAY = 2,
    WEDNESDAY = 3,
    THURSDAY = 4,
    FRIDAY = 5,
    SATURDAY = 6,
    SUNDAY = 7,
} WeekDay;

typedef struct Date {
    WeekDay weekDay;
    uint8_t day;
    uint8_t month; // 1 -12
    uint8_t year;
} Date;

typedef struct Time {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
} Time;

typedef struct DateTime {
    Date date;
    Time time;
} DateTime;

typedef void (*DateChangedCallback)(
        const struct Date *const oldDate, const struct Date *const newDate);

typedef void (*TimeChangedCallback)(
        const struct Time *const oldTime, const struct Time *const newTime);

typedef void (*DateTimeChangedCallback)(
        const struct DateTime *const oldDateTime, const struct DateTime *const newDateTime);

#endif //PICO_CLOCK_TYPES_H

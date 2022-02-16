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


inline bool dateTimeEquals(DateTime a, DateTime b) {
    return a.seconds == b.seconds &&
           a.minutes == b.minutes &&
           a.hours == b.hours &&
           a.weekDay == b.weekDay &&
           a.date == b.date &&
           a.month == b.month &&
           a.year == b.year;
}

inline const char *weekdayToString(WeekDay weekDay) {
    const char *string;
    switch (weekDay) {
        case MONDAY:
            string = "Mon";
            break;
        case TUESDAY:
            string = "Tue";
            break;
        case WEDNESDAY:
            string = "Wed";
            break;
        case THURSDAY:
            string = "Thu";
            break;
        case FRIDAY:
            string = "Fri";
            break;
        case SATURDAY:
            string = "Sat";
            break;
        case SUNDAY:
            string = "Sun";
            break;
    }
    return string;
}

inline const char *monthToString(uint8_t month) {
    if (month < 1 || month > 12) month = 1;
    const char *string;
    switch (month) {
        case 1:
            string = "Jan";
            break;
        case 2:
            string = "Feb";
            break;
        case 3:
            string = "Mar";
            break;
        case 4:
            string = "Apr";
            break;
        case 5:
            string = "May";
            break;
        case 6:
            string = "Jun";
            break;
        case 7:
            string = "Jul";
            break;
        case 8:
            string = "Aug";
            break;
        case 9:
            string = "Sep";
            break;
        case 10:
            string = "Oct";
            break;
        case 11:
            string = "Nov";
            break;
        case 12:
            string = "Dec";
            break;
    }
    return string;
}

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

#ifndef PICO_CLOCK_RTC_HPP
#define PICO_CLOCK_RTC_HPP

class DS1302RTC {

private:
    uint8_t read_register(uint8_t reg);

    void write_register(uint8_t reg, uint8_t value);

public:
    enum WeekDay {
        MONDAY = 1,
        TUESDAY = 2,
        WEDNESDAY = 3,
        THURSDAY = 4,
        FRIDAY = 5,
        SATURDAY = 6,
        SUNDAY = 7,
    };

    struct DateTime {
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours;
        WeekDay weekDay;
        uint8_t date;
        uint8_t month;
        uint8_t year;
    };

    static inline bool date_time_equals(DateTime a, DateTime b) {
        return a.seconds == b.seconds &&
               a.minutes == b.minutes &&
               a.hours == b.hours &&
               a.weekDay == b.weekDay &&
               a.date == b.date &&
               a.month == b.month &&
               a.year == b.year;
    }

    static inline const char *weekday_to_string(WeekDay weekDay) {
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

    static inline const char *month_to_string(uint8_t month) {
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

    void init();

    bool is_running();

    bool is_writable();

    uint get_baud_rate();

    uint8_t get_seconds();

    uint8_t get_minutes();

    uint8_t get_hours();

    uint8_t get_date();

    uint8_t get_month();

    WeekDay get_weekday();

    uint8_t get_year();

    DateTime get_date_time();

    void set_running(bool running);

    void set_writable(bool writable);

    void set_baud_rate(uint baudrate);

    void set_seconds(uint8_t seconds);

    void set_minutes(uint8_t minutes);

    void set_hours(uint8_t hours);

    void set_date(uint8_t date);

    void set_month(uint8_t month);

    void set_weekday(WeekDay weekday);

    void set_year(uint8_t year);
};

#endif //PICO_CLOCK_RTC_HPP

#ifndef PICO_CLOCK_RTC_HPP
#define PICO_CLOCK_RTC_HPP

class DS1302RTC {
    enum WeekDay {
        MONDAY = 1,
        TUESDAY = 2,
        WEDNESDAY = 3,
        THURSDAY = 4,
        FRIDAY = 5,
        SATURDAY = 6,
        SUNDAY = 7,
    };

private:
    uint8_t read_register(uint8_t reg);

    void write_register(uint8_t reg, uint8_t value);

public:
    void init();

    bool is_running();

    bool is_writable();

    uint get_baud_rate();

    uint8_t get_seconds();

    uint8_t get_minutes();

    uint8_t get_hours();

    uint8_t get_date();

    uint8_t get_month();

    uint8_t get_weekday();

    uint8_t get_year();

    void set_running(bool running);

    void set_writable(bool writable);

    void set_baud_rate(bool baudrate);

    void set_seconds(uint8_t seconds);

    void set_minutes(uint8_t minutes);

    void set_hours(uint8_t hours);

    void set_date(uint8_t date);

    void set_month(uint8_t month);

    void set_weekday(WeekDay weekday);

    void set_year(uint8_t year);
};

#endif //PICO_CLOCK_RTC_HPP

#ifndef PICO_CLOCK_RTC_HPP
#define PICO_CLOCK_RTC_HPP

class DS1302RTC {
public:
    void init();

    bool is_halted();

    bool is_writable();

    uint get_baud_rate();

    uint8_t get_seconds();

    uint8_t get_minutes();

    uint8_t get_hours();

    uint8_t get_date();

    uint8_t get_month();

    uint8_t get_weekday();

    uint8_t get_year();

    void read_data();
};

#endif //PICO_CLOCK_RTC_HPP

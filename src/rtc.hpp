#ifndef PICO_CLOCK_RTC_HPP
#define PICO_CLOCK_RTC_HPP

class DS1302RTC {
private:

public:
    void init();

    bool is_halted();

    uint get_baud_rate();

    uint8_t get_seconds();

    void read_data();
};

#endif //PICO_CLOCK_RTC_HPP

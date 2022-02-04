#ifndef PICO_CLOCK_RTC_HPP
#define PICO_CLOCK_RTC_HPP

class DS1302RTC {
private:

public:
    void init();

    uint get_baud_rate();

    void read_data();
};

#endif //PICO_CLOCK_RTC_HPP

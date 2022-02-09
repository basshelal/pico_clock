#ifndef PICO_CLOCK_POWER_MANAGER_HPP
#define PICO_CLOCK_POWER_MANAGER_HPP

#include "ui.hpp"

class PowerManager {
public:
    enum Mode {
        SAVER, NORMAL, REACTIVE
    };

private:
    Mode currentMode;
    uint32_t core0SleepMillis;
    uint32_t core1SleepMillis;

public:
    void init();

    void enter_mode(const Mode mode);

    Mode getCurrentMode();

    uint32_t getCore0SleepMillis();

    uint32_t getCore1SleepMillis();

};

#endif //PICO_CLOCK_POWER_MANAGER_HPP

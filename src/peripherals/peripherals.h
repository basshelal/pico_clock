#ifndef PICO_CLOCK_PERIPHERALS_H
#define PICO_CLOCK_PERIPHERALS_H

#include "../types.h"

extern void peripherals_init();

extern void peripherals_clockSetRunning(bool isRunning);

extern void peripherals_clockSetDateTime(const DateTime *dateTime);

extern void peripherals_clockGetDateTime(DateTime *dateTime);

extern void peripherals_clockSetDateTimeChangedCallback(const DateTimeChangedCallback callback);

extern void peripherals_loop();

#endif //PICO_CLOCK_PERIPHERALS_H

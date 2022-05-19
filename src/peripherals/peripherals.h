#ifndef PICO_CLOCK_PERIPHERALS_H
#define PICO_CLOCK_PERIPHERALS_H

#include "../types.h"

extern void peripherals_init();

extern uint8_t peripherals_displayGetBrightness();

extern void peripherals_displaySetBrightness(uint8_t percentage);

extern void peripherals_clockSetRunning(bool isRunning);

extern void peripherals_clockSetDateTime(const DateTime *dateTime);

extern void peripherals_clockGetDateTime(DateTime *dateTime);

extern void peripherals_clockSetDateChangedCallback(const DateChangedCallback callback);

extern void peripherals_clockSetTimeChangedCallback(const TimeChangedCallback callback);

extern void peripherals_clockSetDateTimeChangedCallback(const DateTimeChangedCallback callback);

extern float peripherals_batteryGetPercentage();

extern float peripherals_batteryGetVoltage();

extern void peripherals_loop();

#endif //PICO_CLOCK_PERIPHERALS_H

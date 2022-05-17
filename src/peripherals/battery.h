#ifndef PICO_CLOCK_BATTERY_H
#define PICO_CLOCK_BATTERY_H

#ifdef __cplusplus
extern "C" {
#endif

extern void battery_init();

extern float battery_getShuntVoltageMilliVolts();

extern float battery_getBusVoltageVolts();

extern float battery_getCurrentMilliAmps();

extern float battery_getPowerMilliWatts();

extern float battery_getPercentage();

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_BATTERY_H

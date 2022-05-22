#ifndef PICO_CLOCK_BATTERY_H
#define PICO_CLOCK_BATTERY_H

#include "../types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void battery_init();

extern void battery_setBatteryStateChangedCallback(const BatteryStateChangedCallback callback);

extern void battery_setBatteryPercentageChangedCallback(const BatteryPercentageChangedCallback callback);

extern float battery_getShuntVoltageMilliVolts();

extern float battery_getBusVoltageVolts();

extern float battery_getCurrentMilliAmps();

extern float battery_getPowerMilliWatts();

extern float battery_getPercentage();

extern void battery_loop();

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_BATTERY_H

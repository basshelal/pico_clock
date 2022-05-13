#ifndef PICO_CLOCK_BATTERY_H
#define PICO_CLOCK_BATTERY_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "constants.h"

#ifdef __cplusplus
extern "C" {
#endif

void battery_init();

float battery_getShuntVoltageMilliVolts();

float battery_getBusVoltageVolts();

float battery_getCurrentMilliAmps();

float battery_getPowerMilliWatts();

float battery_getPercentage();

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_BATTERY_H

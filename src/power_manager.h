#ifndef PICO_CLOCK_POWER_MANAGER_H
#define PICO_CLOCK_POWER_MANAGER_H

#include "battery.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SAVER, NORMAL, REACTIVE
} PowerMode;

void powerManagerInit();

void powerManagerEnterMode(PowerMode mode);

PowerMode powerManagerGetCurrentMode();

void powerManagerLoop();

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_POWER_MANAGER_H

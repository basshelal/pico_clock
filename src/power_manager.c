#include "power_manager.h"

static uint32_t sleepMillis;
static PowerMode currentMode;

void powerManagerInit() {
    powerManagerEnterMode(NORMAL);
}

void powerManagerEnterMode(const PowerMode mode) {
    switch (mode) {
        case SAVER:
            currentMode = SAVER;
            sleepMillis = 200;
            break;
        case NORMAL:
            currentMode = NORMAL;
            sleepMillis = 100;
            break;
        case REACTIVE:
            currentMode = REACTIVE;
            sleepMillis = 20;
            break;
    }
}

PowerMode powerManagerGetCurrentMode() {
    return currentMode;
}

void powerManagerLoop() {
    if (batteryGetPercentage() < 20) {
        powerManagerEnterMode(SAVER);
    }
    sleep_ms(sleepMillis);
}
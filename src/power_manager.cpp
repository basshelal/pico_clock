#include "power_manager.hpp"

void PowerManager::init() {
    enter_mode(NORMAL);
}

void PowerManager::enter_mode(const PowerManager::Mode mode) {
    switch (mode) {
        case SAVER:
            core0SleepMillis = 200;
            core1SleepMillis = 200;
            break;
        case NORMAL:
            core0SleepMillis = 100;
            core1SleepMillis = 100;
            break;
        case REACTIVE:
            core0SleepMillis = 20;
            core1SleepMillis = 20;
            break;
    }
}

PowerManager::Mode PowerManager::getCurrentMode() {
    return currentMode;
}

uint32_t PowerManager::getCore0SleepMillis() {
    return core0SleepMillis;
}

uint32_t PowerManager::getCore1SleepMillis() {
    return core1SleepMillis;
}

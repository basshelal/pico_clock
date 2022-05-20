#include "peripherals.h"
#include "../utils.h"

#include "battery.h"
#include "buttons.h"
#include "display.h"
#include "rtc.h"

public void peripherals_init() {
    display_init();
    buttons_init();
    battery_init();
    rtc_init();
}

public uint8_t peripherals_displayGetBrightness() {
    return display_getBacklight();
}

public void peripherals_displaySetBrightness(uint8_t percentage) {
    display_setBacklight(percentage);
}

public void peripherals_clockSetRunning(bool isRunning) {
    rtc_setIsRunning(isRunning);
}

public void peripherals_clockSetDate(const Date *date) {
    rtc_setDate(date);
}

public void peripherals_clockGetDate(Date *date) {
    rtc_getDate(date);
}

public void peripherals_clockSetTime(const Time *time) {
    rtc_setTime(time);
}

public void peripherals_clockGetTime(Time *time) {
    rtc_getTime(time);
}

public void peripherals_clockSetDateTime(const DateTime *dateTime) {
    rtc_setDateTime(dateTime);
}

public void peripherals_clockGetDateTime(DateTime *dateTime) {
    rtc_getDateTime(dateTime);
}

public void peripherals_clockSetDateChangedCallback(const DateChangedCallback callback) {
    rtc_setDateChangedCallback(callback);
}

public void peripherals_clockSetTimeChangedCallback(const TimeChangedCallback callback) {
    rtc_setTimeChangedCallback(callback);
}

public void peripherals_clockSetDateTimeChangedCallback(const DateTimeChangedCallback callback) {
    rtc_setDateTimeChangedCallback(callback);
}

public float peripherals_batteryGetPercentage() {
    return battery_getPercentage();
}

public float peripherals_batteryGetVoltage() {
    return battery_getBusVoltageVolts();
}

public void peripherals_loop() {
    display_loop();
    buttons_loop();
    battery_loop();
    rtc_loop();
}
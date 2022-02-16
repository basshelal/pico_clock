#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include <stdio.h>

#include "constants.h"
#include "utils.hpp"
#include "rtc.h"
#include "battery.h"
#include "button_handler.h"
#include "ui.h"
#include "power_manager.h"

// Global state variables
static DateTime currentDateTime;
static char timeTextBuffer[9]; // 8 chars in HH:MM:SS format + 1 for string NULL terminator
static char dateTextBuffer[14]; // 13 chars in DOW DD-MMM-YY + 1 for string NULL terminator
static char batteryTextBuffer[64];

// On light pin, useful to check if we are on
static void onLight() {
    gpio_set_dir(ON_LIGHT_PIN, GPIO_OUT);
    gpio_set_function(ON_LIGHT_PIN, GPIO_FUNC_SIO);
    gpio_put(ON_LIGHT_PIN, true);
}

static void requestUpdateToUICore() {
    multicore_fifo_push_blocking(true);
}

static void updateUI() {
    sprintf(timeTextBuffer, "%02i:%02i:%02i",
            currentDateTime.hours, currentDateTime.minutes, currentDateTime.seconds);
    sprintf(dateTextBuffer, "%s %02i-%s-%02i",
            weekdayToString(currentDateTime.weekDay),
            currentDateTime.date, monthToString(currentDateTime.month), currentDateTime.year);
    sprintf(batteryTextBuffer, "%02.f%% %02.02fV %02.fmW %02.02fmV",
            batteryGetPercentage(),
            batteryGetBusVoltageVolts(),
            batteryGetPowerMilliWatts(),
            batteryGetShuntVoltageMilliVolts());
    clear_console();
    log("%s\n", timeTextBuffer);
    log("%s\n", dateTextBuffer);
    log("%s\n", batteryTextBuffer);

    uiShowBatteryPercentage(batteryTextBuffer);
    uiShowClock(timeTextBuffer);
    uiShowDate(dateTextBuffer);

    requestUpdateToUICore();
}

static void loopUICore() {
    int updateRequestCount = 0;
    while (multicore_fifo_rvalid()) {
        bool isDirty = multicore_fifo_pop_blocking();
        if (isDirty) updateRequestCount++;
    }
    if (updateRequestCount > 0) {
        uiUpdate();
        printf("Update requests: %i\n", updateRequestCount);
    }
}

static void launchUICore() {
    while (true) {
        loopUICore();
        sleep_ms(1000 / 30);
    }
}

static void buttonACallback(bool buttonOn) {
    if (buttonOn) uiShowTopLeftButton("A");
    else uiShowTopLeftButton(NULL);
    requestUpdateToUICore();
}

static void buttonBCallback(bool buttonOn) {
    if (buttonOn) uiShowBottomLeftButton("B");
    else uiShowBottomLeftButton(NULL);
    requestUpdateToUICore();
}

static void buttonXCallback(bool buttonOn) {
    if (buttonOn) uiShowTopRightButton("X");
    else uiShowTopRightButton(NULL);
    requestUpdateToUICore();
}

static void buttonYCallback(bool buttonOn) {
    if (buttonOn) uiShowBottomRightButton("Y");
    else uiShowBottomRightButton(NULL);
    requestUpdateToUICore();
}

static void setup() {
    set_sys_clock_48mhz(); // as low as we can reliably go, we do this to save power
    powerManagerInit();

    uiInit(); // first because we will override some gpio pins the screen uses

    uiSetBrightness(100);
    uiUpdate();

    stdio_usb_init();
    onLight();

    buttonHandlerInit();
    buttonHandlerSetCallback(A, buttonACallback);
    buttonHandlerSetCallback(B, buttonBCallback);
    buttonHandlerSetCallback(X, buttonXCallback);
    buttonHandlerSetCallback(Y, buttonYCallback);

    rtcInit();
    rtcSetWritable(true);
    rtcSetRunning(true);

    batteryInit();

    multicore_launch_core1(launchUICore);
}

static void loop() {
    DateTime dateTime = rtcGetDateTime();
    if (!dateTimeEquals(currentDateTime, dateTime)) {
        currentDateTime = dateTime;
        updateUI();
    }
}

int main() {
    setup();
    while (true) {
        loop();
        sleep_ms(1000 / 4);
    }
}

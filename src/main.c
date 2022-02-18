#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include <stdio.h>

#include "constants.h"
#include "utils.h"
#include "rtc.h"
#include "battery.h"
#include "button_handler.h"
#include "ui.h"

// Global state variables
static uint64_t cyclesPassed;
static DateTime currentDateTime;
static DateTime oldDateTime;
static char timeTextBuffer[9]; // 8 chars in HH:MM:SS format + 1 for string NULL terminator
static char dateTextBuffer[14]; // 13 chars in DOW DD-MMM-YY + 1 for string NULL terminator
static char batteryTextBuffer[64];

static ButtonChangedCallback buttonCallbackA;
static ButtonChangedCallback buttonCallbackB;
static ButtonChangedCallback buttonCallbackX;
static ButtonChangedCallback buttonCallbackY;

static ButtonHeldCallback buttonHeldCallbackA;
static ButtonHeldCallback buttonHeldCallbackB;
static ButtonHeldCallback buttonHeldCallbackX;
static ButtonHeldCallback buttonHeldCallbackY;

// On light pin, useful to check if we are on
static void onLight() {
    gpio_set_dir(ON_LIGHT_PIN, GPIO_OUT);
    gpio_set_function(ON_LIGHT_PIN, GPIO_FUNC_SIO);
    gpio_put(ON_LIGHT_PIN, true);
}

static void requestUpdateToUICore() {
    multicore_fifo_push_timeout_us(REQUEST_UPDATE, 5000);
}

static void updateText() {
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
        uint32_t data;
        multicore_fifo_pop_timeout_us(10000, &data);
        if (data == REQUEST_UPDATE) updateRequestCount++;
    }
    if (updateRequestCount > 0) {
        uiUpdate();
        printf("Update requests: %i\n", updateRequestCount);
    }
}

static void launchUICore() {
    while (true) {
        loopUICore();
        sleep_ms(UI_CORE_CYCLE);
    }
}

static void showB(const Button button, const bool buttonOn) {
    if (buttonOn) uiShowBottomLeftButton("B");
    else uiShowBottomLeftButton(NULL);
    requestUpdateToUICore();
}

static void showAAA(const Button button, const uint32_t cyclesHeld, const uint64_t millisHeld) {
    static uint32_t oldCyclesHeld = 16; // any number larger than 1 will do
    static bool isNewHold;
    if (oldCyclesHeld < cyclesHeld) { // we went out of sync, re-sync
        oldCyclesHeld = cyclesHeld;
        isNewHold = false;
    } else if (oldCyclesHeld > cyclesHeld) { // this is a new hold
        oldCyclesHeld = cyclesHeld;
        isNewHold = true;
    }
    if (cyclesHeld > 1 && isNewHold) {
        uiShowTopLeftButton("AAA");
        requestUpdateToUICore();
        isNewHold = false;
    }
    oldCyclesHeld++;
}

static void showXXX(const Button button, const uint32_t cyclesHeld, const uint64_t millisHeld) {
    if (cyclesHeld > 1) {
        uiShowTopRightButton("XYZ");
        requestUpdateToUICore();
    }
}

static void showA(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiShowTopLeftButton("A");
        buttonHeldCallbackA = &showAAA;
    } else {
        uiShowTopLeftButton(NULL);
    }
    requestUpdateToUICore();
}

static void showX(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiShowTopRightButton("X");
    } else {
        uiShowTopRightButton(NULL);
    }
    requestUpdateToUICore();
}

static void showY(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiShowBottomRightButton("Y");
    } else {
        uiShowBottomRightButton(NULL);
    }
    requestUpdateToUICore();
}


static void setup() {
    set_sys_clock_48mhz(); // as low as we can reliably go, we do this to save power

    uiInit(); // first because we will override some gpio pins the screen uses

    uiSetBrightness(100);
    uiUpdate();

    stdio_usb_init();
    onLight();

    buttonHandlerInit();

    buttonCallbackA = &showA;
    buttonCallbackB = &showB;
    buttonCallbackX = &showX;
    buttonCallbackY = &showY;
    buttonHeldCallbackA = &showAAA;
    buttonHeldCallbackX = &showXXX;

    buttonHandlerSetChangedCallback(A, &buttonCallbackA);
    buttonHandlerSetChangedCallback(B, &buttonCallbackB);
    buttonHandlerSetChangedCallback(X, &buttonCallbackX);
    buttonHandlerSetChangedCallback(Y, &buttonCallbackY);

    buttonHandlerSetHeldCallback(A, &buttonHeldCallbackA);
    buttonHandlerSetHeldCallback(B, &buttonHeldCallbackB);
    buttonHandlerSetHeldCallback(X, &buttonHeldCallbackX);
    buttonHandlerSetHeldCallback(Y, &buttonHeldCallbackY);

    rtcInit();
    rtcSetWritable(true);
    rtcSetRunning(true);

    batteryInit();

    multicore_launch_core1(&launchUICore);
}

static void loop() {
    rtcGetDateTime(&currentDateTime);
    if (!dateTimeEquals(&oldDateTime, &currentDateTime)) {
        oldDateTime = currentDateTime;
        updateText();
    }
    buttonHandlerLoop();
    uiLoop();
}

int main() {
    setup();
    while (true) {
        loop();
        sleep_ms(MAIN_CORE_CYCLE);
        cyclesPassed++;
    }
}

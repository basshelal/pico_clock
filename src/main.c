#include "pico/stdlib.h"
#include "hardware/irq.h"
#include <stdio.h>

#include "constants.h"
#include "utils.h"
#include "rtc.h"
#include "battery.h"
#include "ui.h"
#include "ui_controller.h"

// Global state variables
static DateTime currentDateTime;
static DateTime oldDateTime;
static char timeTextBuffer[9]; // 8 chars in HH:MM:SS format + 1 for string NULL terminator
static char dateTextBuffer[14]; // 13 chars in DOW DD-MMM-YY + 1 for string NULL terminator
static char batteryTextBuffer[64];

// On light pin, useful to check if we are on
static void onLight() {
    gpio_set_dir(ON_LIGHT_PIN, GPIO_OUT);
    gpio_set_function(ON_LIGHT_PIN, GPIO_FUNC_SIO);
    gpio_put(ON_LIGHT_PIN, true);
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

    uiRequestUpdate();
}

static void setup() {
    set_sys_clock_48mhz(); // as low as we can reliably go, we do this to save power

    uiControllerInit();
    stdio_usb_init();
    onLight();

    rtcInit();
    rtcSetWritable(true);
    rtcSetRunning(true);

    batteryInit();
}

static void loop() {
    rtcGetDateTime(&currentDateTime);
    if (!dateTimeEquals(&oldDateTime, &currentDateTime)) {
        oldDateTime = currentDateTime;
        updateText();
    }
    uiControllerLoop();
}

int main() {
    setup();
    while (true) {
        loop();
        sleep_ms(MAIN_CORE_CYCLE);
        cyclesPassed++;
    }
}

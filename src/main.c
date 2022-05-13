#include "pico/stdlib.h"
#include "hardware/irq.h"
#include <stdio.h>

#include "constants.h"
#include "utils.h"
#include "rtc.h"
#include "battery.h"
#include "ui_view.h"
#include "ui_controller.h"

// Global state variables
private DateTime currentDateTime;
private DateTime oldDateTime;
private char timeTextBuffer[9]; // 8 chars in HH:MM:SS format + 1 for string NULL terminator
private char dateTextBuffer[14]; // 13 chars in DOW DD-MMM-YY + 1 for string NULL terminator
private char batteryTextBuffer[64];

// On light pin, useful to check if we are on
private void onLight() {
    gpio_set_dir(ON_LIGHT_PIN, GPIO_OUT);
    gpio_set_function(ON_LIGHT_PIN, GPIO_FUNC_SIO);
    gpio_put(ON_LIGHT_PIN, true);
}

private void updateText() {
    sprintf(timeTextBuffer, "%02i:%02i:%02i",
            currentDateTime.hours, currentDateTime.minutes, currentDateTime.seconds);
    sprintf(dateTextBuffer, "%s %02i-%s-%02i",
            rtc_weekdayToString(currentDateTime.weekDay),
            currentDateTime.date, rtc_monthToString(currentDateTime.month), currentDateTime.year);
    sprintf(batteryTextBuffer, "%02.f%% %02.02fV %02.fmW %02.02fmV",
            battery_getPercentage(),
            battery_getBusVoltageVolts(),
            battery_getPowerMilliWatts(),
            battery_getShuntVoltageMilliVolts());
    clear_console();
    log("%s\n", timeTextBuffer);
    log("%s\n", dateTextBuffer);
    log("%s\n", batteryTextBuffer);

    uiView_showBatteryPercentage(batteryTextBuffer);
    uiView_showClock(timeTextBuffer);
    uiView_showDate(dateTextBuffer);

    uiView_requestUpdate();
}

private void setup() {
    set_sys_clock_48mhz(); // as low as we can reliably go, we do this to save power
    stdio_usb_init();

    onLight();

    uiController_init();

    rtc_init();
    if (!rtc_isRunning()) rtc_setIsRunning(true);

    battery_init();
}

private void loop() {
    rtc_getDateTime(&currentDateTime);
    if (!rtc_dateTimeEquals(&oldDateTime, &currentDateTime)) {
        oldDateTime = currentDateTime;
        updateText();
    }
    uiController_loop();
}

public int main() {
    setup();
    while (true) {
        loop();
        sleep_ms(MILLIS_PER_CYCLE_MAIN_CORE);
        finishCycle();
    }
}

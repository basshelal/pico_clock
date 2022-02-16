#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include <stdio.h>

#include "constants.h"
#include "utils.hpp"
#include "rtc.hpp"
#include "battery.h"
#include "button_handler.h"
#include "ui.hpp"
#include "power_manager.h"

// TODO: 09-Feb-2022 @basshelal: Use a consistent formatting, I think I will stick with Java style

// Global state variables
static DS1302RTC rtc = DS1302RTC();
static DS1302RTC::DateTime currentDateTime = DS1302RTC::DateTime();
static char timeTextBuffer[9]; // 8 chars in HH:MM:SS format + 1 for string NULL terminator
static char dateTextBuffer[14]; // 13 chars in DOW DD-MMM-YY + 1 for string NULL terminator
static char batteryTextBuffer[64];
static UI ui = UI();

// On light pin, useful to check if we are on
static void on_light() {
    gpio_set_dir(ON_LIGHT_PIN, GPIO_OUT);
    gpio_set_function(ON_LIGHT_PIN, GPIO_FUNC_SIO);
    gpio_put(ON_LIGHT_PIN, true);
}

static void update_screen() {
    sprintf(timeTextBuffer, "%02i:%02i:%02i",
            currentDateTime.hours, currentDateTime.minutes, currentDateTime.seconds);
    sprintf(dateTextBuffer, "%s %02i-%s-%02i",
            DS1302RTC::weekday_to_string(currentDateTime.weekDay),
            currentDateTime.date, DS1302RTC::month_to_string(currentDateTime.month), currentDateTime.year);
    sprintf(batteryTextBuffer, "%02.f%% %02.02fV %02.fmW %02.02fmV",
            batteryGetPercentage(),
            batteryGetBusVoltageVolts(),
            batteryGetPowerMilliWatts(),
            batteryGetShuntVoltageMilliVolts());
    clear_console();
    log("%s\n", timeTextBuffer);
    log("%s\n", dateTextBuffer);
    log("%s\n", batteryTextBuffer);

    ui.show_battery_percentage(batteryTextBuffer);
    ui.show_clock(timeTextBuffer);
    ui.show_date(dateTextBuffer);

    ui.update();
}

static void core1_loop() {

}

static void launch_core1() {
    while (true) {
        core1_loop();
        sleep_ms(32);
    }
}

static void buttonACallback(bool buttonOn) {
    if (buttonOn) ui.show_top_left_button("A");
    else ui.show_top_left_button(NULL);
}

static void buttonBCallback(bool buttonOn) {
    if (buttonOn) ui.show_bottom_left_button("B");
    else ui.show_bottom_left_button(NULL);
}

static void buttonXCallback(bool buttonOn) {
    if (buttonOn) ui.show_top_right_button("X");
    else ui.show_top_right_button(NULL);
}

static void buttonYCallback(bool buttonOn) {
    if (buttonOn) ui.show_bottom_right_button("Y");
    else ui.show_bottom_right_button(NULL);
}

static void setup() {
    set_sys_clock_48mhz(); // as low as we can reliably go, we do this to save power
    powerManagerInit();

    ui.init(); // first because we will override some gpio pins the screen uses

    ui.set_brightness(100);
    ui.update();

    stdio_usb_init();
    on_light();

    buttonHandlerInit();
    buttonHandlerSetCallback(A, buttonACallback);
    buttonHandlerSetCallback(B, buttonBCallback);
    buttonHandlerSetCallback(X, buttonXCallback);
    buttonHandlerSetCallback(Y, buttonYCallback);

    rtc.init();
    rtc.set_writable(true);
    rtc.set_running(true);

    batteryInit();

    multicore_launch_core1(launch_core1);
}

static void loop() {
    DS1302RTC::DateTime dateTime = rtc.get_date_time();
    if (!DS1302RTC::date_time_equals(currentDateTime, dateTime)) {
        currentDateTime = dateTime;
        update_screen();
    }
}

int main() {
    setup();
    while (true) {
        loop();
        powerManagerLoop();
    }
}

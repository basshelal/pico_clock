#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <stdio.h>

#include "constants.hpp"
#include "utils.hpp"
#include "rtc.hpp"
#include "battery.hpp"
#include "button_handler.hpp"
#include "ui.hpp"
#include "power_manager.hpp"

// Global state variables
static DS1302RTC rtc = DS1302RTC();
static DS1302RTC::DateTime currentDateTime = DS1302RTC::DateTime();
static INA219 battery = INA219();
static char timeTextBuffer[9]; // 8 chars in HH:MM:SS format + 1 for string NULL terminator
static char dateTextBuffer[14]; // 13 chars in DOW DD-MMM-YY + 1 for string NULL terminator
static char batteryTextBuffer[64];
static ButtonHandler buttonHandler = ButtonHandler();
static UI ui = UI();
static PowerManager powerManager = PowerManager();

static int core0SleepMillis = 100;
static int core1SleepMillis = 1000 / 60;

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
            battery.get_percentage(),
            battery.getBusVoltage_V(),
            battery.getPower_mW(),
            battery.getShuntVoltage_mV());
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
        sleep_ms(powerManager.getCore1SleepMillis());
    }
}

static bool stateA = false;

static void setup() {
    set_sys_clock_48mhz(); // as low as we can reliably go, we do this to save power
    powerManager.init();

    ui.init(); // first because we will override some gpio pins the screen uses

    ui.set_brightness(100);
    ui.update();

    stdio_usb_init();
    on_light();

    buttonHandler.init();

    rtc.init();
    rtc.set_writable(true);
    rtc.set_running(true);

    battery.init();
    battery.powerSave(true);

    //multicore_launch_core1(launch_core1);

    buttonHandler.set_button_callback(ButtonHandler::A, [](bool on) {
        if (on && !stateA) {
            ui.show_top_left_button("A", true);
            stateA = true;
            ui.update();
        } else if (!on && stateA) {
            ui.show_top_left_button("", false);
            stateA = false;
            ui.update();
        }
    });
}

static void loop() {
    buttonHandler.read();

    DS1302RTC::DateTime dateTime = rtc.get_date_time();
    if (!DS1302RTC::date_time_equals(currentDateTime, dateTime)) {
        currentDateTime = dateTime;
        update_screen();
    }
    if (battery.get_percentage() < 20) {
        powerManager.enter_mode(PowerManager::SAVER);
    }
}

int main() {
    setup();
    while (true) {
        loop();
        sleep_ms(powerManager.getCore0SleepMillis());
    }
}

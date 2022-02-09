#include "pico/stdlib.h"
#include "hardware/irq.h"
#include <stdio.h>

#include "constants.hpp"
#include "utils.hpp"
#include "rtc.hpp"
#include "battery.hpp"
#include "button_handler.hpp"
#include "ui.hpp"

// Global state variables
static DS1302RTC rtc = DS1302RTC();
static DS1302RTC::DateTime currentDateTime = DS1302RTC::DateTime();
static INA219 battery = INA219();
static char timeTextBuffer[9]; // 8 chars in HH:MM:SS format + 1 for string NULL terminator
static char dateTextBuffer[13]; // 12 chars in DOW DD-MM-YY + 1 for string NULL terminator
static char batteryTextBuffer[64];
static ButtonHandler buttonHandler = ButtonHandler();
static UI ui = UI();

// On light pin, useful to check if we are on
static void on_light() {
    gpio_set_dir(ON_LIGHT_PIN, GPIO_OUT);
    gpio_set_function(ON_LIGHT_PIN, GPIO_FUNC_SIO);
    gpio_put(ON_LIGHT_PIN, true);
}

static void update_screen() {
    irq_clear(SPI1_IRQ);
    sprintf(timeTextBuffer, "%02i:%02i:%02i",
            currentDateTime.hours, currentDateTime.minutes, currentDateTime.seconds);
    sprintf(dateTextBuffer, "%s %02i-%02i-%02i",
            DS1302RTC::weekday_to_string(currentDateTime.weekDay),
            currentDateTime.date, currentDateTime.month, currentDateTime.year);
    sprintf(batteryTextBuffer, "%02.f%% V: %02.02f mW: %02.02f mV: %02.02f",
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

    ui.update();
}

static void setup() {
    set_sys_clock_48mhz(); // as low as we can reliably go, we do this to save power

    ui.init(); // first because we will override some gpio pins the screen uses

    ui.set_brightness(100);
    ui.update();

    stdio_usb_init();
    on_light();

    buttonHandler.init();

    rtc.init();
    rtc.set_writable(true);
    rtc.set_running(true);

    rtc.set_seconds(42);
    rtc.set_minutes(42);
    rtc.set_hours(6);
    rtc.set_weekday(DS1302RTC::SATURDAY);
    rtc.set_month(6);
    rtc.set_date(6);
    rtc.set_year(69);

    battery.init();
    battery.powerSave(true);
}

static void loop() {
    buttonHandler.read();

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
        sleep_ms(100);
    }
}

#include "pico/stdlib.h"
#include "hardware/irq.h"
#include <stdio.h>

#include <font8_data.hpp>
#include "pico_display_2.hpp"

#include "constants.hpp"
#include "utils.hpp"
#include "rtc.hpp"
#include "battery.hpp"
#include "screen.hpp"
#include "button_handler.hpp"

// Global state variables
static DS1302RTC rtc = DS1302RTC();
static DS1302RTC::DateTime currentDateTime = DS1302RTC::DateTime();
static INA219 battery = INA219();
static PimoroniScreen screen = PimoroniScreen();
static char timeTextBuffer[64];
static char dateTextBuffer[64];
static char batteryTextBuffer[64];
static ButtonHandler buttonHandler = ButtonHandler();

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
    sprintf(batteryTextBuffer, "mW: %02.02f mA: %02.02f mV: %02.02f V: %02.02f %02.02f%%\n",
            battery.getPower_mW(),
            battery.getCurrent_mA(),
            battery.getShuntVoltage_mV(),
            battery.getBusVoltage_V(),
            battery.get_percentage());
    clear_console();
    log("%s\n", timeTextBuffer);
    log("%s\n", dateTextBuffer);
    log("%s\n", batteryTextBuffer);
    // at 13:55 it is displaying: mW 320 mA -81  mV -0.81 V 4.13

    screen.clear();
    screen.display.text(timeTextBuffer, Point(25, 50), PimoroniScreen::WIDTH - 25, 4);
    screen.display.text(dateTextBuffer, Point(25, 100), PimoroniScreen::WIDTH - 25, 4);
    screen.display.text(batteryTextBuffer, Point(25, 150), PimoroniScreen::WIDTH - 25, 2);
    screen.update();
}

static void setup() {
    set_sys_clock_48mhz(); // as low as we can reliably go

    screen.init(); // first because we will override some gpio pins it uses
    screen.clear();
    screen.update();
    screen.display.set_pen(255, 255, 255);
    screen.display.set_font(&font8);

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

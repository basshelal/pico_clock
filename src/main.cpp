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

// Global state variables
static struct {
    bool A;
    bool B;
    bool X;
    bool Y;
} screen_buttons;

static DS1302RTC rtc = DS1302RTC();
static DS1302RTC::DateTime currentDateTime = DS1302RTC::DateTime();
static INA219 battery = INA219();
static PimoroniScreen screen = PimoroniScreen();
static char textBuffer[512];

// On light pin, useful to check if we are on
static void on_light() {
    gpio_set_dir(ON_LIGHT_PIN, /*out=*/ true);
    gpio_set_function(ON_LIGHT_PIN, GPIO_FUNC_SIO);
    gpio_put(ON_LIGHT_PIN, true);
}

static void initialize_screen_buttons() {
    gpio_set_dir(A_BUTTON_PIN, GPIO_IN);
    gpio_set_function(A_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_pull_up(A_BUTTON_PIN);

    gpio_set_dir(B_BUTTON_PIN, GPIO_IN);
    gpio_set_function(B_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_pull_up(B_BUTTON_PIN);

    gpio_set_dir(X_BUTTON_PIN, GPIO_IN);
    gpio_set_function(X_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_pull_up(X_BUTTON_PIN);

    gpio_set_dir(Y_BUTTON_PIN, GPIO_IN);
    gpio_set_function(Y_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_pull_up(Y_BUTTON_PIN);
}

static void read_buttons() {
    screen_buttons.A = !gpio_get(A_BUTTON_PIN);
    screen_buttons.B = !gpio_get(B_BUTTON_PIN);
    screen_buttons.X = !gpio_get(X_BUTTON_PIN);
    screen_buttons.Y = !gpio_get(Y_BUTTON_PIN);
}

static void updateScreenInterruptHandler() {
    irq_clear(SPI1_IRQ);
    sprintf(textBuffer, "%02i:%02i:%02i %s %02i-%02i-%02i",
            currentDateTime.hours, currentDateTime.minutes, currentDateTime.seconds,
            DS1302RTC::weekday_to_string(currentDateTime.weekDay),
            currentDateTime.date, currentDateTime.month, currentDateTime.year);
    clear_console();
    log("%s\n", textBuffer);

    screen.clear();
    screen.display.text(textBuffer, Point(25, 70), PimoroniScreen::WIDTH - 25);
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
    initialize_screen_buttons();

    //  irq_set_enabled(SPI1_IRQ, true);
    //  irq_set_exclusive_handler(SPI1_IRQ, updateScreenInterruptHandler);
    //  irq_set_priority(SPI1_IRQ, PICO_HIGHEST_IRQ_PRIORITY);

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
}

static void loop() {
    read_buttons();

    DS1302RTC::DateTime dateTime = rtc.get_date_time();
    if (!DS1302RTC::date_time_equals(currentDateTime, dateTime)) {
        currentDateTime = dateTime;
        updateScreenInterruptHandler();
        //    irq_set_pending(SPI1_IRQ);
    }
}

int main() {
    setup();
    while (true) {
        loop();
        sleep_ms(100);
    }
}

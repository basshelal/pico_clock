#include "pico/stdlib.h"
#include <stdio.h>
#include "pico_display_2.hpp"

#include "constants.hpp"
#include "utils.hpp"
#include "rtc.hpp"

using namespace pimoroni;

// Global state variables
static uint16_t buffer[PicoDisplay2::WIDTH * PicoDisplay2::HEIGHT];
static PicoDisplay2 pico_display(buffer);
static struct {
    bool A;
    bool B;
    bool X;
    bool Y;
} screen_buttons;

static DS1302RTC rtc = DS1302RTC();

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

static void initialize_rtc() {
    rtc.init();
}

static void read_buttons() {
    screen_buttons.A = !gpio_get(A_BUTTON_PIN);
    screen_buttons.B = !gpio_get(B_BUTTON_PIN);
    screen_buttons.X = !gpio_get(X_BUTTON_PIN);
    screen_buttons.Y = !gpio_get(Y_BUTTON_PIN);
}

static void setup() {
    pico_display.init(); // first because we will override some gpio pins it uses
    // screen off
    pico_display.set_backlight(0);
    pico_display.update();

    stdio_usb_init();
    on_light();
    initialize_screen_buttons();
    initialize_rtc();
}

static void loop() {
    read_buttons();
    log("%02i:%02i:%02i %02i %02i %02i %02i\n",
        rtc.get_hours(), rtc.get_minutes(), rtc.get_seconds(),
        rtc.get_weekday(), rtc.get_date(), rtc.get_month(), rtc.get_year());
}

int main() {
    setup();
    while (true) {
        loop();
        sleep_ms(1000);
    }
}

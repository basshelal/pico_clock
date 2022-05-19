#include "peripherals.h"
#include "../utils.h"

#include "battery.h"
#include "buttons.h"
#include "display.h"
#include "rtc.h"

public void peripherals_init() {
    rtc_init();
    battery_init();
    display_init();
    buttons_init();
}

public void peripherals_loop() {
    rtc_loop();
    battery_loop();
    display_loop();
    buttons_loop();
}
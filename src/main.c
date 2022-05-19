#include "pico/stdlib.h"
#include "hardware/irq.h"
#include <stdio.h>

#include "constants.h"
#include "utils.h"
#include "ui/ui.h"
#include "peripherals/peripherals.h"

// On light pin, useful to check if we are on
private void onLight() {
    gpio_set_dir(ON_LIGHT_PIN, GPIO_OUT);
    gpio_set_function(ON_LIGHT_PIN, GPIO_FUNC_SIO);
    gpio_put(ON_LIGHT_PIN, true);
}

private void init() {
    set_sys_clock_48mhz(); // as low as we can reliably go, we do this to save power
    stdio_usb_init();

    if (isDebug()) onLight();

    peripherals_init();
    ui_init();
}

private void loop() {
    peripherals_loop();
    ui_loop();
}

public int main() {
    init();
    while (true) {
        loop();
        sleep_ms(MILLIS_PER_CYCLE_MAIN_CORE);
        finishCycle();
    }
}

#include "pico/stdlib.h"
#include <stdio.h>

static const int onLightPin = PICO_DEFAULT_LED_PIN;

static int counter = 0;

// On light pin, useful to check if we are on
static void onLight() {
    gpio_set_dir(onLightPin, /*out=*/ true);
    gpio_set_function(onLightPin, GPIO_FUNC_SIO);
    gpio_put(onLightPin, true);
}

static void setup() {
    stdio_usb_init();
    onLight();
}

static void loop() {
    printf("Hello world %i\n", counter++);
    sleep_ms(1000);
}

int main() {
    setup();
    while (true) loop();
}

#ifndef PICO_CLOCK_SCREEN_HPP
#define PICO_CLOCK_SCREEN_HPP

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico_display_2.hpp"

using namespace pimoroni;

class PimoroniScreen {
public:
    static const int WIDTH = PicoDisplay2::WIDTH;
    static const int HEIGHT = PicoDisplay2::HEIGHT;
    static const int AREA = WIDTH * HEIGHT;
public:
    uint16_t buffer[AREA];
    PicoDisplay2 display = PicoDisplay2(buffer);

    void init();

    void set_brightness(const uint8_t brightness);

    void clear();

    void update();

    void set_pixel(const int x, const int y,
                   const uint8_t r, const uint8_t g, const uint8_t b);

};

#endif //PICO_CLOCK_SCREEN_HPP

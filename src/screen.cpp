#include <font8_data.hpp>
#include "screen.hpp"

void PimoroniScreen::init() {
    display.init();
}

void PimoroniScreen::set_brightness(const uint8_t brightness) {
    display.set_backlight(brightness);
}

void PimoroniScreen::clear() {
    for (int i = 0; i < AREA; ++i) {
        buffer[i] = 0;
    }
}

void PimoroniScreen::update() {
    display.update();
}

static inline uint16_t color(const uint8_t r, const uint8_t g, const uint8_t b) {
    uint16_t p = ((b & 0b11111000) << 8) |
                 ((r & 0b11111100) << 3) |
                 ((g & 0b11111000) >> 3);
    return p;
};

void PimoroniScreen::set_pixel(const int x, const int y,
                               const uint8_t r, const uint8_t g, const uint8_t b) {
    display.set_pen(r, g, b);
    //display.pixel(Point(x,y));

    display.set_font(&font8);
    display.text("SUCK MY ASS!", Point(50, 50), WIDTH);
}
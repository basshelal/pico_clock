#include "display.h"
#include "pico_graphics.hpp"
#include "pico_display_2.hpp"
#include "custom_font.hpp"
#include "utils.hpp"

using namespace pimoroni;

static PicoDisplay2 display = nullptr;

void displayInit(uint16_t *buffer) {
    display = PicoDisplay2(buffer);
    display.init();
    displayClear();
    display.set_font(&custom_font);
}

void displayClear() {
    for (int i = 0; i < DISPLAY_AREA; ++i) {
        display.__fb[i] = 0;
    }
}

void displaySetText(const char *text, int32_t x, int32_t y, int32_t wrap, uint8_t scale) {
    display.text(text, Point(x, y), wrap, scale);
}

void displayUpdate() {
    display.update();
}

void displaySetBacklight(const uint8_t percentage) {
    const float factor = percentage > 100 ? 1.0F : ((float) percentage) / 100.0F;
    const uint8_t brightness = factor == 0.0F ? 0 : (uint8_t) (255.0F * factor);
    display.set_backlight(brightness);
}

void displaySetColor(const Color color) {
    display.set_pen(color.r, color.g, color.b);
}

void displaySetRectangle(const Rectangle rectangle) {
    display.rectangle(Rect(rectangle.x, rectangle.y, rectangle.w, rectangle.h));
}

uint8_t displayGetCharWidth(const char c) {
    return custom_font.widths[get_font_index_for_char(c)];
}

int displayGetStringWidth(const char *string) {
    int result = 0;
    for (int i = 0; i < strlen(string); ++i) {
        result += displayGetCharWidth(string[i]);
    }
    return result;
}
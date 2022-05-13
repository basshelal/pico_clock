#include "display.h"
#include "pico_graphics.hpp"
#include "pico_display_2.hpp"
#include <font8_data.hpp>
#include "utils.h"

using namespace pimoroni;

private PicoDisplay2 display = NULL;
private uint8_t brightnessPercentage;

public void display_init(uint16_t *const buffer) {
    display = PicoDisplay2(buffer);
    display.init();
    display_clear();
    display.set_font(&font8);
}

public void display_clear() {
    for (int i = 0; i < DISPLAY_AREA; ++i) {
        display.__fb[i] = 0;
    }
}

public void display_update() {
    display.update();
}

public void display_setColor(const Color color) {
    display.set_pen(color.r, color.g, color.b);
}

public void display_setText(const char *text, const int32_t x, const int32_t y,
                            const int32_t wrap, const uint8_t scale) {
    display.text(text, Point(x, y), wrap, scale);
}

public void display_setRectangle(const Rectangle rectangle) {
    display.rectangle(Rect(rectangle.x, rectangle.y, rectangle.w, rectangle.h));
}

public void display_setBacklight(const uint8_t percentage) {
    if (percentage < 0 || percentage > 100) return;
    const float factor = percentage > 100 ? 1.0F : ((float) percentage) / 100.0F;
    const uint8_t brightness = factor == 0.0F ? 0 : (uint8_t) (255.0F * factor);
    display.set_backlight(brightness);
    brightnessPercentage = percentage;
}

public uint8_t display_getBacklight() {
    return brightnessPercentage;
}

public int display_getStringWidth(const char *string, const int scale) {
    return bitmap::measure_text(&font8, string, scale);
}

public Color display_getRandomColor() {
    Color result;
    result.r = (uint8_t) random();
    result.g = (uint8_t) random();
    result.b = (uint8_t) random();
    return result;
}
#include "ui.hpp"

static inline uint16_t color(const uint8_t r, const uint8_t g, const uint8_t b) {
    uint16_t p = ((b & 0b11111000) << 8) |
                 ((r & 0b11111100) << 3) |
                 ((g & 0b11111000) >> 3);
    return p;
}

static inline Pen WHITE() { return color(255, 255, 255); }

static inline Pen BLACK() { return color(0, 0, 0); }

static inline Pen RED() { return color(255, 0, 0); }

static inline Pen GREEN() { return color(0, 255, 0); }

static inline Pen BLUE() { return color(0, 0, 255); }

void UI::clear_all() {
    for (int i = 0; i < AREA; ++i) {
        buffer[i] = 0;
    }
}

void UI::set_rect(const Rect &rect, Pen color) {
    display->set_pen(color);
    display->rectangle(rect);
    display->set_pen(WHITE());
}

void UI::clear_rect(const Rect &rect) {
    set_rect(rect, BLACK());
}

uint8_t UI::get_char_width(const char c) {
    return font8.widths[get_font_index_for_char(c)];
}

int UI::get_string_width(const char *string) {
    int result = 0;
    for (int i = 0; i < strlen(string); ++i) {
        result += get_char_width(string[i]);
    }
    return result;
}

void UI::init() {
    display->init();
    clear_all();
    display->set_font(&font8);
    display->set_pen(WHITE());
}

void UI::show_battery_percentage(const char *text, const bool isShowing) {
    set_rect(batteryRect, RED());
    if (isShowing) {
        display->text(text, Point(batteryRect.x, batteryRect.y),
                      WIDTH - outsideMargin, batteryTextScale);
    }
}

void UI::show_clock(const char *text, const bool isShowing) {
    set_rect(clockRect, BLACK());
    if (isShowing) {
        clockRect.w = (get_string_width(text) * clockTextScale);
        set_rect(clockRect, GREEN());
        display->text(text, Point(clockRect.x, clockRect.y),
                      WIDTH - outsideMargin, clockTextScale);
    }
}

void UI::show_date(const char *text, const bool isShowing) {

}

void UI::show_message(const char *text, const bool isShowing) {

}

void UI::show_top_left_button(const char *text, const bool isShowing) {

}

void UI::show_bottom_left_button(const char *text, const bool isShowing) {

}

void UI::show_top_right_button(const char *text, const bool isShowing) {

}

void UI::show_bottom_right_button(const char *text, const bool isShowing) {

}

void UI::set_brightness(const uint8_t percentage) {
    const float factor = percentage > 100 ? 1.0F : ((float) percentage) / 100.0F;
    const uint8_t brightness = factor == 0.0F ? 0 : (uint8_t) (255.0F * factor);
    log("factor: %f, brightness: %u\n", factor, brightness);
    display->set_backlight(brightness);
}

void UI::update() {
    display->update();
}

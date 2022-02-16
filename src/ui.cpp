#include "ui.hpp"

// Copied from pico_graphics.hpp
static inline uint16_t color(const uint8_t r, const uint8_t g, const uint8_t b) {
    uint16_t p = ((r & 0b11111000) << 8) |
                 ((g & 0b11111100) << 3) |
                 ((b & 0b11111000) >> 3);
    return __builtin_bswap16(p);
}

static inline Pen WHITE() { return color(255, 255, 255); }

static inline Pen BLACK() { return color(0, 0, 0); }

static inline Pen GREY() { return color(127, 127, 127); }

static inline Pen RED() { return color(255, 0, 0); }

static inline Pen GREEN() { return color(0, 255, 0); }

static inline Pen BLUE() { return color(0, 0, 255); }

static inline Pen YELLOW() { return color(255, 255, 0); }

static inline Pen CYAN() { return color(0, 255, 255); }

static inline Pen MAGENTA() { return color(255, 0, 255); }

static inline Pen PINK() { return color(255, 0, 127); }

static inline Pen LIME() { return color(127, 255, 0); }

static inline Pen VIOLET() { return color(127, 0, 255); }

static inline Pen ORANGE() { return color(255, 127, 0); }

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
    return custom_font.widths[get_font_index_for_char(c)];
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
    display->set_font(&custom_font);
    display->set_pen(WHITE());
}

void UI::show_battery_percentage(const char *text) {
    set_rect(batteryRect, BLACK());
    if (text != nullptr) {
        batteryRect.w = (get_string_width(text) * batteryTextScale);
        display->text(text, Point(batteryRect.x, batteryRect.y),
                      WIDTH - outsideMargin, batteryTextScale);
    }
}

void UI::show_clock(const char *text) {
    set_rect(clockRect, BLACK());
    if (text != nullptr) {
        clockRect.w = (get_string_width(text) * clockTextScale);
        clockRect.x = (WIDTH / 2) - (clockRect.w / 2);
        display->text(text, Point(clockRect.x, clockRect.y),
                      WIDTH - outsideMargin, clockTextScale);
    }
}

void UI::show_date(const char *text) {
    set_rect(dateRect, BLACK());
    if (text) {
        dateRect.w = (get_string_width(text) * dateTextScale);
        dateRect.x = (WIDTH / 2) - (dateRect.w / 2);
        display->text(text, Point(dateRect.x, dateRect.y),
                      WIDTH - outsideMargin, dateTextScale);
    }
}

void UI::show_message(const char *text) {
    set_rect(messageRect, BLACK());
    if (text != nullptr) {
        const int wrapWidth = WIDTH - (2 * 4 * charWidth * buttonTextScale);
        const int fullLineWidth = (get_string_width(text) * messageTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        messageRect.w = lineWidth;
        messageRect.x = (WIDTH / 2) - (messageRect.w / 2);
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        messageRect.h = (lines * charHeight * messageTextScale) + (lineBreaks * messageTextScale);
        display->text(text, Point(messageRect.x, messageRect.y),
                      wrapWidth, messageTextScale);
    }
}

void UI::show_top_left_button(const char *text) {
    set_rect(topLeftButtonRect, BLACK());
    if (text != nullptr) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
        const int fullLineWidth = (get_string_width(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        topLeftButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        topLeftButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        display->text(text, Point(topLeftButtonRect.x, topLeftButtonRect.y),
                      wrapWidth, buttonTextScale);
    }
}

void UI::show_bottom_left_button(const char *text) {
    set_rect(bottomLeftButtonRect, BLACK());
    if (text != nullptr) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
        const int fullLineWidth = (get_string_width(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        bottomLeftButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        bottomLeftButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        display->text(text, Point(bottomLeftButtonRect.x, bottomLeftButtonRect.y),
                      wrapWidth, buttonTextScale);
    }
}

void UI::show_top_right_button(const char *text) {
    set_rect(topRightButtonRect, BLACK());
    if (text != nullptr) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
        const int fullLineWidth = (get_string_width(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        topRightButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        topRightButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        display->text(text, Point(topRightButtonRect.x, topRightButtonRect.y),
                      wrapWidth, buttonTextScale);
    }
}

void UI::show_bottom_right_button(const char *text) {
    set_rect(bottomRightButtonRect, BLACK());
    if (text != nullptr) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
        const int fullLineWidth = (get_string_width(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        bottomRightButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        bottomRightButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        display->text(text, Point(bottomRightButtonRect.x, bottomRightButtonRect.y),
                      wrapWidth, buttonTextScale);
    }
}

void UI::set_brightness(const uint8_t percentage) {
    const float factor = percentage > 100 ? 1.0F : ((float) percentage) / 100.0F;
    const uint8_t brightness = factor == 0.0F ? 0 : (uint8_t) (255.0F * factor);
    log("factor: %f, brightness: %u\n", factor, brightness);
    display->set_backlight(brightness);
}

void UI::update() {
    display->update();
    log("UI update!\n");
}

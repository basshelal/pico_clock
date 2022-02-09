#ifndef PICO_CLOCK_UI_HPP
#define PICO_CLOCK_UI_HPP

#include "pico_graphics.hpp"
#include "font8_data.hpp"
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico_display_2.hpp"
#include "utils.hpp"
#include "custom_font.hpp"

using namespace pimoroni;

class UI {
public:
    static const int WIDTH = PicoDisplay2::WIDTH;
    static const int HEIGHT = PicoDisplay2::HEIGHT;
    static const int AREA = WIDTH * HEIGHT;

    static const int outsideMargin = 10;
    static const int charWidth = 6;
    static const int charHeight = 8;

private:
    uint16_t *buffer = (uint16_t *) malloc(AREA * sizeof(uint16_t));

    PicoDisplay2 *display = new PicoDisplay2(buffer);

    static const int batteryTextScale = 1;

    Rect batteryRect = Rect(outsideMargin, outsideMargin,
                            4 * charWidth * batteryTextScale, batteryTextScale * charHeight);

    static const int clockTextScale = 6;

    Rect clockRect = Rect(outsideMargin * clockTextScale, outsideMargin * 6,
                          6 * charWidth * clockTextScale, 1 * charHeight * clockTextScale);

    static const int dateTextScale = 2;

    Rect dateRect = Rect(outsideMargin * clockTextScale, outsideMargin * 14,
                         12 * charWidth * dateTextScale, 1 * charHeight * dateTextScale);

    static const int messageTextScale = 2;

    Rect messageRect = Rect(outsideMargin * clockTextScale, outsideMargin * 18,
                            12 * charWidth * messageTextScale, 3 * charHeight * messageTextScale);

    static const int buttonTextScale = 2;

    Rect topLeftButtonRect = Rect(0, HEIGHT / 6,
                                  4 * charWidth * buttonTextScale, 2 * charHeight * buttonTextScale);

    Rect topRightButtonRect = Rect(WIDTH - (4 * charWidth * buttonTextScale), HEIGHT / 6,
                                   4 * charWidth * buttonTextScale, 2 * charHeight * buttonTextScale);

    Rect bottomLeftButtonRect = Rect(0, 4 * (HEIGHT / 6),
                                     4 * charWidth * buttonTextScale, 2 * charHeight * buttonTextScale);

    Rect bottomRightButtonRect = Rect(WIDTH - (4 * charWidth * buttonTextScale), 4 * (HEIGHT / 6),
                                      4 * charWidth * buttonTextScale, 2 * charHeight * buttonTextScale);

    void clear_all();

    void set_rect(const Rect &rect, Pen color);

    void clear_rect(const Rect &rect);

    uint8_t get_char_width(const char c);

    int get_string_width(const char *string);

public:

    void init();

    void show_battery_percentage(const char *text,
                                 const bool isShowing = true);

    void show_clock(const char *text,
                    const bool isShowing = true);

    void show_date(const char *text,
                   const bool isShowing = true);

    void show_message(const char *text,
                      const bool isShowing = true);

    void show_top_left_button(const char *text,
                              const bool isShowing = true);

    void show_bottom_left_button(const char *text,
                                 const bool isShowing = true);

    void show_top_right_button(const char *text,
                               const bool isShowing = true);

    void show_bottom_right_button(const char *text,
                                  const bool isShowing = true);

    void set_brightness(const uint8_t percentage);

    void update();

    // battery percentage at top left
    // middle large clock
    // bottom smaller full date
    // a button to enter change brightness mode
    //    change brightness mode has 4 buttons, increase, decrease, cancel, accept
    //    as you change the brightness percentage is displayed
    // a button to enter set time mode
    //    in this mode you use a cursor to select which field you would like to modify
    //    there is a button to move left, move right, select field, and finish setting
    //    when field is selected, you have an increase button, decrease button and finish button
    //    when setting, the clock should be stopped and the field being modified should be
    //    shown using flashing or something obvious
};

#endif //PICO_CLOCK_UI_HPP

#ifndef PICO_CLOCK_UI_HPP
#define PICO_CLOCK_UI_HPP


#include "screen.hpp"

class UI {
private:
    PimoroniScreen screen;

public:
    UI(PimoroniScreen &_screen) { screen = _screen; };

    void update_battery_percentage(const float batteryPercent,
                                   const bool isShowing = true);

    void update_clock(const char *clockString,
                      const bool isShowing = true);

    void update_date(const char *dateString,
                     const bool isShowing = true);

    void update_message(const char *messageText,
                        const bool isShowing = true);

    void update_top_left_button(const char *text,
                                const bool isShowing = true);

    void update_bottom_left_button(const char *text,
                                   const bool isShowing = true);

    void update_top_right_button(const char *text,
                                 const bool isShowing = true);

    void update_bottom_right_button(const char *text,
                                    const bool isShowing = true);

    void update_screen();

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

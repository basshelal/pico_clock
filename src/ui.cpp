#include "ui.hpp"

void UI::update_battery_percentage(const float batteryPercent, const bool isShowing) {

}

void UI::update_clock(const char *clockString, const bool isShowing) {
    // TODO: 08-Feb-2022 @basshelal: Clear only the area that has the clock :/
    if (isShowing) {
        screen.display.text(clockString,
                            Point(PimoroniScreen::WIDTH / 10, PimoroniScreen::HEIGHT / 10),
                            PimoroniScreen::WIDTH - (PimoroniScreen::WIDTH / 10), 4);
    }
}

void UI::update_date(const char *dateString, const bool isShowing) {

}

void UI::update_message(const char *messageText, const bool isShowing) {

}

void UI::update_top_left_button(const char *text, const bool isShowing) {

}

void UI::update_bottom_left_button(const char *text, const bool isShowing) {

}

void UI::update_top_right_button(const char *text, const bool isShowing) {

}

void UI::update_bottom_right_button(const char *text, const bool isShowing) {

}

void UI::update_screen() {
    screen.update();
}

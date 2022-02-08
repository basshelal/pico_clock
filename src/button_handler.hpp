#ifndef PICO_CLOCK_BUTTON_HANDLER_HPP
#define PICO_CLOCK_BUTTON_HANDLER_HPP

#include "constants.hpp"
#include "pico/stdlib.h"

typedef void (*ButtonCallback)(void);

class ButtonHandler {
public:
    enum Button {
        A, B, X, Y
    };

private:
    bool onA;
    bool onB;
    bool onX;
    bool onY;

    ButtonCallback callbackA;
    ButtonCallback callbackB;
    ButtonCallback callbackX;
    ButtonCallback callbackY;

public:
    void init();

    void read();

    void set_button_callback(const Button button, const ButtonCallback callback);
};

#endif //PICO_CLOCK_BUTTON_HANDLER_HPP

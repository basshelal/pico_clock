#include "button_handler.hpp"

void ButtonHandler::init() {
    gpio_set_dir(A_BUTTON_PIN, GPIO_IN);
    gpio_set_function(A_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_pull_up(A_BUTTON_PIN);

    gpio_set_dir(B_BUTTON_PIN, GPIO_IN);
    gpio_set_function(B_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_pull_up(B_BUTTON_PIN);

    gpio_set_dir(X_BUTTON_PIN, GPIO_IN);
    gpio_set_function(X_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_pull_up(X_BUTTON_PIN);

    gpio_set_dir(Y_BUTTON_PIN, GPIO_IN);
    gpio_set_function(Y_BUTTON_PIN, GPIO_FUNC_SIO);
    gpio_pull_up(Y_BUTTON_PIN);
}

void ButtonHandler::read() {
    onA = !gpio_get(A_BUTTON_PIN);
    onB = !gpio_get(B_BUTTON_PIN);
    onX = !gpio_get(X_BUTTON_PIN);
    onY = !gpio_get(Y_BUTTON_PIN);

    if (onA && callbackA) callbackA();
    if (onB && callbackB) callbackB();
    if (onX && callbackX) callbackX();
    if (onY && callbackY) callbackY();
}

void ButtonHandler::set_button_callback(const ButtonHandler::Button button,
                                        const ButtonCallback callback) {
    switch (button) {
        case A:
            callbackA = callback;
            break;
        case B:
            callbackB = callback;
            break;
        case X:
            callbackX = callback;
            break;
        case Y:
            callbackY = callback;
            break;
    }
}

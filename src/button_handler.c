#include "button_handler.h"

static ButtonCallback callbackA;
static ButtonCallback callbackB;
static ButtonCallback callbackX;
static ButtonCallback callbackY;

static void handleCallback(uint gpio, uint32_t event) {
    switch (gpio) {
        case A_BUTTON_PIN:
            if (callbackA) callbackA(!gpio_get(gpio));
            break;
        case B_BUTTON_PIN:
            if (callbackB) callbackB(!gpio_get(gpio));
            break;
        case X_BUTTON_PIN:
            if (callbackX) callbackX(!gpio_get(gpio));
            break;
        case Y_BUTTON_PIN:
            if (callbackY) callbackY(!gpio_get(gpio));
            break;
    }
}

void buttonHandlerInit() {
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

    gpio_set_irq_enabled_with_callback(A_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, handleCallback);
    gpio_set_irq_enabled_with_callback(B_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, handleCallback);
    gpio_set_irq_enabled_with_callback(X_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, handleCallback);
    gpio_set_irq_enabled_with_callback(Y_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, handleCallback);
}

void buttonHandlerSetCallback(const Button button, const ButtonCallback callback) {
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

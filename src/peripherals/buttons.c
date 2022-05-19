#include "buttons.h"
#include "../utils.h"
#include "pico/stdlib.h"

public ButtonState buttonStateA = {.button = A_BUTTON_PIN};
public ButtonState buttonStateB = {.button = B_BUTTON_PIN};
public ButtonState buttonStateX = {.button = X_BUTTON_PIN};
public ButtonState buttonStateY = {.button = Y_BUTTON_PIN};

private inline void handleStateChanged(ButtonState *const state) {
    if (!state) return;
    state->isOn = !gpio_get(state->button);
    if (state->changedCallback) {
        (state->changedCallback)(state);
    }
}

private void handleCallback(const uint gpio, const uint32_t event) {
    switch (gpio) {
        case A_BUTTON_PIN:
            handleStateChanged(&buttonStateA);
            break;
        case B_BUTTON_PIN:
            handleStateChanged(&buttonStateB);
            break;
        case X_BUTTON_PIN:
            handleStateChanged(&buttonStateX);
            break;
        case Y_BUTTON_PIN:
            handleStateChanged(&buttonStateY);
            break;
    }
}

private inline void handleHeld(ButtonState *const state) {
    if (!state) return;
    if (state->isOn) {
        state->millisHeld += MILLIS_PER_CYCLE_MAIN_CORE;
        if (state->millisHeld > 0 && state->heldCallback) {
            (state->heldCallback)(state);
        }
    } else {
        state->millisHeld = -MILLIS_PER_CYCLE_MAIN_CORE;
    }
}

public void buttons_init() {
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

    // Skip 1 cycle before a hold is triggered, otherwise a single press will count as a short hold
    buttonStateA.millisHeld = -MILLIS_PER_CYCLE_MAIN_CORE;
    buttonStateB.millisHeld = -MILLIS_PER_CYCLE_MAIN_CORE;
    buttonStateX.millisHeld = -MILLIS_PER_CYCLE_MAIN_CORE;
    buttonStateY.millisHeld = -MILLIS_PER_CYCLE_MAIN_CORE;
}

public void buttons_loop() {
    handleHeld(&buttonStateA);
    handleHeld(&buttonStateB);
    handleHeld(&buttonStateX);
    handleHeld(&buttonStateY);
}
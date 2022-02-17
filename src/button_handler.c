#include "button_handler.h"

static struct ButtonState {
    const Button button;
    bool isOn;
    int millisHeld;
    ButtonChangedCallback *changedCallback;
    ButtonHeldCallback *heldCallback;
} ButtonState;

static struct ButtonState stateA = {.button = A_BUTTON_PIN};
static struct ButtonState stateB = {.button = B_BUTTON_PIN};
static struct ButtonState stateX = {.button = X_BUTTON_PIN};
static struct ButtonState stateY = {.button = Y_BUTTON_PIN};

static inline void handleStateChanged(struct ButtonState *const state) {
    state->isOn = !gpio_get(state->button);
    if (state->changedCallback && (*state->changedCallback)) {
        (*state->changedCallback)(state->button, state->isOn);
    }
}

static void handleCallback(uint gpio, uint32_t event) {
    switch (gpio) {
        case A_BUTTON_PIN:
            handleStateChanged(&stateA);
            break;
        case B_BUTTON_PIN:
            handleStateChanged(&stateB);
            break;
        case X_BUTTON_PIN:
            handleStateChanged(&stateX);
            break;
        case Y_BUTTON_PIN:
            handleStateChanged(&stateY);
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

    // Skip 1 cycle before a hold is triggered, otherwise a single press will count as a short hold
    stateA.millisHeld = -MAIN_CORE_CYCLE;
    stateB.millisHeld = -MAIN_CORE_CYCLE;
    stateX.millisHeld = -MAIN_CORE_CYCLE;
    stateY.millisHeld = -MAIN_CORE_CYCLE;
}

void buttonHandlerSetChangedCallback(const Button button, const ButtonChangedCallback *const callback) {
    switch (button) {
        case A:
            stateA.changedCallback = callback;
            break;
        case B:
            stateB.changedCallback = callback;
            break;
        case X:
            stateX.changedCallback = callback;
            break;
        case Y:
            stateY.changedCallback = callback;
            break;
    }
}

void buttonHandlerSetHeldCallback(const Button button, const ButtonHeldCallback *const callback) {
    switch (button) {
        case A:
            stateA.heldCallback = callback;
            break;
        case B:
            stateB.heldCallback = callback;
            break;
        case X:
            stateX.heldCallback = callback;
            break;
        case Y:
            stateY.heldCallback = callback;
            break;
    }
}

static inline void handleHeld(struct ButtonState *const state) {
    if (state->isOn) {
        state->millisHeld += MAIN_CORE_CYCLE;
        if (state->millisHeld > 0 && state->heldCallback && (*state->heldCallback)) {
            (*state->heldCallback)(state->button, state->millisHeld / MAIN_CORE_CYCLE, state->millisHeld);
        }
    } else {
        state->millisHeld = -MAIN_CORE_CYCLE;
    }
}

void buttonHandlerLoop() {
    handleHeld(&stateA);
    handleHeld(&stateB);
    handleHeld(&stateX);
    handleHeld(&stateY);
}
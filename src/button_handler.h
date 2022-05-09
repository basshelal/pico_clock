#ifndef PICO_CLOCK_BUTTON_HANDLER_H
#define PICO_CLOCK_BUTTON_HANDLER_H

#include "constants.h"
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    A, B, X, Y
} Button;

struct ButtonState;

typedef void (*ButtonChangedCallback)(const struct ButtonState *const buttonState);
typedef void (*ButtonHeldCallback)(const Button button, const uint32_t cyclesHeld, const uint64_t millisHeld);

typedef struct ButtonState {
    const Button button;
    bool isOn;
    int millisHeld;
    ButtonChangedCallback changedCallback;
    ButtonHeldCallback heldCallback;
} ButtonState;

extern ButtonState buttonStateA;
extern ButtonState buttonStateB;
extern ButtonState buttonStateX;
extern ButtonState buttonStateY;

void buttonHandlerInit();

void buttonHandlerLoop();

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_BUTTON_HANDLER_H

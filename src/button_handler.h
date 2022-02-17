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

typedef void (*ButtonChangedCallback)(const Button button, const bool buttonOn);
typedef void (*ButtonHeldCallback)(const Button button, const int cyclesHeld, const int millisHeld);

void buttonHandlerInit();

void buttonHandlerSetChangedCallback(const Button button, const ButtonChangedCallback callback);

void buttonHandlerSetHeldCallback(const Button button, const ButtonHeldCallback callback);

void buttonHandlerLoop();

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_BUTTON_HANDLER_H

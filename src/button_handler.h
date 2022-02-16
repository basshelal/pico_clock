#ifndef PICO_CLOCK_BUTTON_HANDLER_H
#define PICO_CLOCK_BUTTON_HANDLER_H

#include "constants.h"
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ButtonCallback)(bool buttonOn);

typedef enum {
    A, B, X, Y
} Button;

void buttonHandlerInit();

void buttonHandlerSetCallback(const Button button, const ButtonCallback callback);

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_BUTTON_HANDLER_H

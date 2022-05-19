#ifndef PICO_CLOCK_BUTTONS_H
#define PICO_CLOCK_BUTTONS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    A, B, X, Y
} Button;

struct ButtonState;

typedef void (*ButtonChangedCallback)(const struct ButtonState *const buttonState);
typedef void (*ButtonHeldCallback)(const struct ButtonState *const buttonState);

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

extern void buttons_init();

extern void buttons_loop();

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_BUTTONS_H

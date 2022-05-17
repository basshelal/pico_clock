#ifndef PICO_CLOCK_UI_MODEL_H
#define PICO_CLOCK_UI_MODEL_H

#include "../utils.h"
#include "../types.h"

typedef enum UIActivity {
    HIDDEN, DETAILS,
    SET_BRIGHTNESS, SET_TIME,
    SET_DATE, SET_ALARM
} UIActivity;

extern void uiModel_init();

extern void uiModel_buttonPressed();

extern const UIActivity uiModel_getActivity();

extern void uiModel_setActivity(const UIActivity activity);

extern void uiModel_incrementBrightness();

extern void uiModel_decrementBrightness();

extern void uiModel_incrementClockHighlight();

extern void uiModel_decrementRightClockHighlight();

extern void uiModel_loop();

#endif //PICO_CLOCK_UI_MODEL_H
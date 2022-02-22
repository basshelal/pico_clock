#ifndef PICO_CLOCK_UI_H
#define PICO_CLOCK_UI_H

#include <stdlib.h>
#include "pico/stdlib.h"
#include <pico/multicore.h>
#include <stdio.h>
#include "display.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

Color color(uint8_t r, uint8_t g, uint8_t b);
extern const Color WHITE;
extern const Color BLACK;
extern const Color RED;
extern const Color GREEN;
extern const Color BLUE;
extern const Color YELLOW;

void uiInit();

void uiShowBatteryPercentage(const char *text);

void uiShowClock(const char *text);

void uiShowDate(const char *text);

void uiShowMessage(const char *text);

void uiShowTopLeftButton(const char *text);

void uiShowColoredTopLeftButton(const char *text, const Color textColor);

void uiShowBottomLeftButton(const char *text);

void uiShowTopRightButton(const char *text);

void uiShowBottomRightButton(const char *text);

void uiSetBrightness(const uint8_t percentage);

void uiRequestUpdate();

void uiForceUpdate();

void uiLoop();

// battery percentage at top left
// middle large clock
// bottom smaller full date
// a button to enter change brightness mode
//    change brightness mode has 4 buttons, increase, decrease, cancel, accept
//    as you change the brightness percentage is displayed
// a button to enter set time mode
//    in this mode you use a cursor to select which field you would like to modify
//    there is a button to move left, move right, select field, and finish setting
//    when field is selected, you have an increase button, decrease button and finish button
//    when setting, the clock should be stopped and the field being modified should be
//    shown using flashing or something obvious

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_UI_H

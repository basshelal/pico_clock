#ifndef PICO_CLOCK_UI_VIEW_H
#define PICO_CLOCK_UI_VIEW_H

#include <stdlib.h>
#include "pico/stdlib.h"
#include <pico/multicore.h>
#include <stdio.h>
#include "display.h"
#include "utils.h"

/**
 * The UIView is a single module that is responsible for any manipulation of the screen.
 * Thus, it abstracts away the lower level details of modifying the screen's buffer directly
 */

#ifdef __cplusplus
extern "C" {
#endif

extern const Color WHITE;
extern const Color BLACK;
extern const Color RED;
extern const Color GREEN;
extern const Color BLUE;
extern const Color YELLOW;

/** Call once before UIView use */
void uiView_init();

/** Clears the entire UI */
void uiView_clearAll();

/** Clears the details of the UI, leaving the core UI elements, the clock and date */
void uiView_clearDetails();

void uiView_showBatteryPercentage(const char *text);

void uiView_showClock(const char *text);

void uiView_showDate(const char *text);

void uiView_showMessage(const char *text);

void uiView_showTopLeftButton(const char *text);

void uiView_showColoredTopLeftButton(const char *text, const Color textColor);

void uiView_showBottomLeftButton(const char *text);

void uiView_showTopRightButton(const char *text);

void uiView_showBottomRightButton(const char *text);

void uiView_setBrightness(const uint8_t percentage);

/** Request that the UIView tells the screen to refresh at its earliest convenience */
void uiView_requestUpdate();

/** Force screen to refresh, use only when needed, use uiView_requestUpdate() for all other cases */
void uiView_forceUpdate();

/** Called once during the main loop */
void uiView_loop();

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

#endif //PICO_CLOCK_UI_VIEW_H

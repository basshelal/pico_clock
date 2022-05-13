#ifndef PICO_CLOCK_UI_CONTROLLER_H
#define PICO_CLOCK_UI_CONTROLLER_H

/**
 * Bridges the UIView (ui_view.h) with the Button Handler (button_handler.h)
 */

#include "button_handler.h"
#include "ui_view.h"

/** Call once to initialize UIController */
void uiController_init();

/** Call once in main loop */
void uiController_loop();

#endif //PICO_CLOCK_UI_CONTROLLER_H

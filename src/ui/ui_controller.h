#ifndef PICO_CLOCK_UI_CONTROLLER_H
#define PICO_CLOCK_UI_CONTROLLER_H

/**
 * Bridges the UIView (ui_view.h) with the Button Handler (button_handler.h)
 */

/** Call once to initialize UIController */
extern void uiController_init();

/** Call once in main loop */
extern void uiController_loop();

#endif //PICO_CLOCK_UI_CONTROLLER_H

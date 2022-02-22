#ifndef PICO_CLOCK_UI_CONTROLLER_H
#define PICO_CLOCK_UI_CONTROLLER_H

#include "button_handler.h"
#include "ui.h"

void uiControllerInit();

void uiControllerLoop();

void _showInitialButtonFunctions(const Button button, const bool buttonOn);

#endif //PICO_CLOCK_UI_CONTROLLER_H

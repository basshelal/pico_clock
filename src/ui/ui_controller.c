#include "ui_controller.h"
#include "../utils.h"
#include "../peripherals/buttons.h"
#include "ui_view.h"
#include "ui_model.h"

private void uiController_pressedA() {
    uiModel_buttonPressed();
    switch (uiModel_getActivity()) {
        case HIDDEN:
            uiModel_setActivity(DETAILS);
            break;
        case DETAILS:
            uiModel_setActivity(SET_BRIGHTNESS);
            break;
        case SET_BRIGHTNESS:
            break;
        case SET_TIME:
            uiModel_incrementClockHighlight();
            break;
        case SET_DATE:
            uiModel_decrementDateHighlight();
            break;
        case SET_ALARM:
            break;
    }
}

private void uiController_pressedB() {
    uiModel_buttonPressed();
    switch (uiModel_getActivity()) {
        case HIDDEN:
            uiModel_setActivity(DETAILS);
            break;
        case DETAILS:
            uiModel_setActivity(SET_TIME);
            break;
        case SET_BRIGHTNESS:
            break;
        case SET_TIME:
            uiModel_cancelSetTime();
            break;
        case SET_DATE:
            uiModel_incrementDateHighlight();
            break;
        case SET_ALARM:
            break;
    }
}

private void uiController_pressedX() {
    uiModel_buttonPressed();
    switch (uiModel_getActivity()) {
        case HIDDEN:
            uiModel_setActivity(DETAILS);
            break;
        case DETAILS:
            uiModel_setActivity(SET_DATE);
            break;
        case SET_BRIGHTNESS:
            uiModel_incrementBrightness();
            break;
        case SET_TIME:
            uiModel_incrementSetTime();
            break;
        case SET_DATE:
            break;
        case SET_ALARM:
            break;
    }
}

private void uiController_pressedY() {
    uiModel_buttonPressed();
    switch (uiModel_getActivity()) {
        case HIDDEN:
            uiModel_setActivity(DETAILS);
            break;
        case DETAILS:
            break;
        case SET_BRIGHTNESS:
            uiModel_decrementBrightness();
            break;
        case SET_TIME:
            uiModel_acceptSetTime();
            break;
        case SET_DATE:
            break;
        case SET_ALARM:
            break;
    }
}

private void uiController_changedCallbackA(const ButtonState *const buttonState) {
    if (buttonState->isOn) uiController_pressedA();
}

private void uiController_heldCallbackA(const ButtonState *const buttonState) {
    if (buttonState->millisHeld >= 500) uiController_pressedA();
}

private void uiController_changedCallbackB(const ButtonState *const buttonState) {
    if (buttonState->isOn) uiController_pressedB();
}

private void uiController_heldCallbackB(const ButtonState *const buttonState) {
    if (buttonState->millisHeld >= 500) uiController_pressedB();
}

private void uiController_changedCallbackX(const ButtonState *const buttonState) {
    if (buttonState->isOn) uiController_pressedX();
}

private void uiController_heldCallbackX(const ButtonState *const buttonState) {
    if (buttonState->millisHeld >= 500) uiController_pressedX();
}

private void uiController_changedCallbackY(const ButtonState *const buttonState) {
    if (buttonState->isOn) uiController_pressedY();
}

private void uiController_heldCallbackY(const ButtonState *const buttonState) {
    if (buttonState->millisHeld >= 500) uiController_pressedY();
}

public void uiController_init() {
    uiModel_init();
    buttons_init();

    buttonStateA.changedCallback = uiController_changedCallbackA;
    buttonStateA.heldCallback = uiController_heldCallbackA;

    buttonStateB.changedCallback = uiController_changedCallbackB;
    buttonStateB.heldCallback = uiController_heldCallbackB;

    buttonStateX.changedCallback = uiController_changedCallbackX;
    buttonStateX.heldCallback = uiController_heldCallbackX;

    buttonStateY.changedCallback = uiController_changedCallbackY;
    buttonStateY.heldCallback = uiController_heldCallbackY;
}

public void uiController_loop() {
    buttons_loop();
    uiModel_loop();
}
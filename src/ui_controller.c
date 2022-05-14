#include "ui_controller.h"

private struct {
    bool countingMillisSinceLastPress;
    uint64_t millisSinceLastPress;
    enum {
        HIDDEN, DETAILS,
        SET_BRIGHTNESS, SET_TIME,
        SET_DATE, SET_ALARM
    } currentMode;
    uint64_t timeoutMillis;
#define DEFAULT_TIMEOUT_MILLIS 3500
} state;

private char messageBuffer[512];

private void uiController_showDetails() {
    state.countingMillisSinceLastPress = true;
    state.millisSinceLastPress = 0;

    state.currentMode = DETAILS;
    uiView_showTopLeftButton("SET BRIT");
    uiView_showBottomLeftButton("SET TIME");
    uiView_showTopRightButton("SET DATE");
    uiView_showBottomRightButton("SET ALRM");
    uiView_requestUpdate();
}

private void uiController_clearDetails() {
    state.countingMillisSinceLastPress = false;
    state.millisSinceLastPress = 0;
    state.timeoutMillis = DEFAULT_TIMEOUT_MILLIS;

    state.currentMode = HIDDEN;
    uiView_clearDetails();
    uiView_hideClockHighlight();
    uiView_requestUpdate();
}

private void uiController_pressedA() {
    state.millisSinceLastPress = 0;
    switch (state.currentMode) {
        case HIDDEN:
            uiController_showDetails();
            break;
        case DETAILS:
            state.currentMode = SET_BRIGHTNESS;
            uiView_showTopRightButton("+");
            uiView_showBottomRightButton("-");
            uiView_showTopLeftButton(NULL);
            uiView_showBottomLeftButton(NULL);

            snprintf(messageBuffer, 512, "%u %%", display_getBacklight());
            uiView_showMessage(messageBuffer);
            uiView_requestUpdate();
            break;
        case SET_BRIGHTNESS:
            break;
        case SET_TIME:
            break;
        case SET_DATE:
            break;
        case SET_ALARM:
            break;
    }
}

private void uiController_pressedB() {
    state.millisSinceLastPress = 0;
    switch (state.currentMode) {
        case HIDDEN:
            uiController_showDetails();
            break;
        case DETAILS:
            state.currentMode = SET_TIME;
            state.timeoutMillis = 5000;
            uiView_showTopLeftButton("<");
            uiView_showBottomLeftButton(">");
            uiView_showTopRightButton("+");
            uiView_showBottomRightButton("-");
            uiView_showMessage("Set time");

            uiView_showClockHighlight(0, 3, RED);

            uiView_requestUpdate();
            break;
        case SET_BRIGHTNESS:
            break;
        case SET_TIME:
            break;
        case SET_DATE:
            break;
        case SET_ALARM:
            break;
    }
}

private void uiController_pressedX() {
    state.millisSinceLastPress = 0;
    switch (state.currentMode) {
        case HIDDEN:
            uiController_showDetails();
            break;
        case DETAILS:
            break;
        case SET_BRIGHTNESS:
            display_setBacklight(display_getBacklight() + 5);
            snprintf(messageBuffer, 512, "%u %%", display_getBacklight());
            uiView_showMessage(messageBuffer);
            uiView_requestUpdate();
            break;
        case SET_TIME:
            break;
        case SET_DATE:
            break;
        case SET_ALARM:
            break;
    }
}

private void uiController_pressedY() {
    state.millisSinceLastPress = 0;
    switch (state.currentMode) {
        case HIDDEN:
            uiController_showDetails();
            break;
        case DETAILS:
            break;
        case SET_BRIGHTNESS:
            display_setBacklight(display_getBacklight() - 5);
            snprintf(messageBuffer, 512, "%u %%", display_getBacklight());
            uiView_showMessage(messageBuffer);
            uiView_requestUpdate();
            break;
        case SET_TIME:
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
    uiView_init();
    buttonHandler_init();

    buttonStateA.changedCallback = uiController_changedCallbackA;
    buttonStateA.heldCallback = uiController_heldCallbackA;

    buttonStateB.changedCallback = uiController_changedCallbackB;
    buttonStateB.heldCallback = uiController_heldCallbackB;

    buttonStateX.changedCallback = uiController_changedCallbackX;
    buttonStateX.heldCallback = uiController_heldCallbackX;

    buttonStateY.changedCallback = uiController_changedCallbackY;
    buttonStateY.heldCallback = uiController_heldCallbackY;

    state.timeoutMillis = DEFAULT_TIMEOUT_MILLIS;
}

public void uiController_loop() {
    buttonHandler_loop();
    uiView_loop();

    if (state.countingMillisSinceLastPress) {
        state.millisSinceLastPress += MILLIS_PER_CYCLE_MAIN_CORE;
    }
    if (state.millisSinceLastPress > state.timeoutMillis) {
        uiController_clearDetails();
    }
}
#include "ui_controller.h"

private struct {
    bool isShowing;
    uint64_t millisSinceLastPress;
    bool countingMillisSinceLastPress;
    enum {
        NONE, SET_TIME, SET_BRIGHTNESS
    } currentMode;
    int pressCountA;
} state;

private char buffer[512];

private void changedCallbackButtonA(const ButtonState *const buttonState) {
    if (state.isShowing) {
        if (buttonState->isOn) {
            state.countingMillisSinceLastPress = true;
            state.millisSinceLastPress = 0;
            state.pressCountA++;

            snprintf(buffer, 512, "Pressed A %i times", state.pressCountA);
            uiView_showMessage(buffer);

            uiView_requestUpdate();
        }
    } else if (!state.isShowing) {
        if (buttonState->isOn) {
            state.countingMillisSinceLastPress = true;
            state.millisSinceLastPress = 0;
            state.pressCountA++;

            state.isShowing = true;
            uiView_showTopLeftButton("A");

            snprintf(buffer, 512, "Pressed A %i times", state.pressCountA);
            uiView_showMessage(buffer);

            uiView_requestUpdate();
        }
    }
}

public void uiController_init() {
    uiView_init();
    buttonHandler_init();

    buttonStateA.changedCallback = changedCallbackButtonA;
}

public void uiController_loop() {
    buttonHandler_loop();
    uiView_loop();

    if (state.countingMillisSinceLastPress) {
        state.millisSinceLastPress += MILLIS_PER_CYCLE_MAIN_CORE;
    }
    if (state.millisSinceLastPress > 2000) {
        state.countingMillisSinceLastPress = false;
        state.millisSinceLastPress = 0;

        state.isShowing = false;
        uiView_clearDetails();

        state.pressCountA = 0;
    }
}
#include "ui_controller.h"

static struct {
    bool isShowing;
    uint64_t millisSinceLastPress;
    bool countingMillisSinceLastPress;
    enum {
        NONE, SET_TIME, SET_BRIGHTNESS
    } currentMode;
    int pressCountA;
} state;

char buffer[512];

static void changedCallbackButtonA(const ButtonState *const buttonState) {
    if (state.isShowing) {
        if (buttonState->isOn) {
            state.countingMillisSinceLastPress = true;
            state.millisSinceLastPress = 0;
            state.pressCountA++;

            snprintf(buffer, 512, "Pressed A %i times", state.pressCountA);
            uiViewShowMessage(buffer);

            uiViewRequestUpdate();
        }
    } else if (!state.isShowing) {
        if (buttonState->isOn) {
            state.countingMillisSinceLastPress = true;
            state.millisSinceLastPress = 0;
            state.pressCountA++;

            state.isShowing = true;
            uiViewShowTopLeftButton("A");

            snprintf(buffer, 512, "Pressed A %i times", state.pressCountA);
            uiViewShowMessage(buffer);

            uiViewRequestUpdate();
        }
    }
}

void uiControllerInit() {
    uiViewInit();
    buttonHandlerInit();

    buttonStateA.changedCallback = changedCallbackButtonA;
}

void uiControllerLoop() {
    buttonHandlerLoop();
    uiViewLoop();

    if (state.countingMillisSinceLastPress) {
        state.millisSinceLastPress += MILLIS_PER_CYCLE_MAIN_CORE;
    }
    if (state.millisSinceLastPress > 2000) {
        state.countingMillisSinceLastPress = false;
        state.millisSinceLastPress = 0;

        state.isShowing = false;
        uiViewClearDetails();

        state.pressCountA = 0;
    }
}
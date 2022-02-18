#include "ui_controller.h"

static struct {
    ButtonChangedCallback A;
    ButtonChangedCallback B;
    ButtonChangedCallback X;
    ButtonChangedCallback Y;
} changedCallbacks;

static struct {
    ButtonHeldCallback A;
    ButtonHeldCallback B;
    ButtonHeldCallback X;
    ButtonHeldCallback Y;
} heldCallbacks;

static void showB(const Button button, const bool buttonOn) {
    if (buttonOn) uiShowBottomLeftButton("B");
    else uiShowBottomLeftButton(NULL);
    uiRequestUpdate();
}

static void showAAA(const Button button, const uint32_t cyclesHeld, const uint64_t millisHeld) {
    static uint32_t oldCyclesHeld = 16; // any number larger than 1 will do
    static bool isNewHold;
    if (oldCyclesHeld < cyclesHeld) { // we went out of sync, re-sync
        oldCyclesHeld = cyclesHeld;
        isNewHold = false;
    } else if (oldCyclesHeld > cyclesHeld) { // this is a new hold
        oldCyclesHeld = cyclesHeld;
        isNewHold = true;
    }
    if (cyclesHeld > 1 && isNewHold) {
        uiShowTopLeftButton("AAA");
        uiRequestUpdate();
        isNewHold = false;
    }
    oldCyclesHeld++;
}

static void showXXX(const Button button, const uint32_t cyclesHeld, const uint64_t millisHeld) {
    if (cyclesHeld > 1) {
        uiShowTopRightButton("XYZ");
        uiRequestUpdate();
    }
}

static void showA(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiShowTopLeftButton("A");
        heldCallbacks.A = &showAAA;
    } else {
        uiShowTopLeftButton(NULL);
    }
    uiRequestUpdate();
}

static void showX(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiShowTopRightButton("X");
    } else {
        uiShowTopRightButton(NULL);
    }
    uiRequestUpdate();
}

static void showY(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiShowBottomRightButton("Y");
    } else {
        uiShowBottomRightButton(NULL);
    }
    uiRequestUpdate();
}

void uiControllerInit() {
    uiInit();
    uiSetBrightness(100);
    buttonHandlerInit();

    changedCallbacks.A = &showA;
    changedCallbacks.B = &showB;
    changedCallbacks.X = &showX;
    changedCallbacks.Y = &showY;
    heldCallbacks.A = &showAAA;
    heldCallbacks.X = &showXXX;

    buttonHandlerSetChangedCallback(A, &changedCallbacks.A);
    buttonHandlerSetChangedCallback(B, &changedCallbacks.B);
    buttonHandlerSetChangedCallback(X, &changedCallbacks.X);
    buttonHandlerSetChangedCallback(Y, &changedCallbacks.Y);

    buttonHandlerSetHeldCallback(A, &heldCallbacks.A);
    buttonHandlerSetHeldCallback(B, &heldCallbacks.B);
    buttonHandlerSetHeldCallback(X, &heldCallbacks.X);
    buttonHandlerSetHeldCallback(Y, &heldCallbacks.Y);
}

void uiControllerLoop() {
    buttonHandlerLoop();
    uiLoop();
}
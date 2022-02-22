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

static void showA(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiShowTopLeftButton("A");
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

static void clickedSetDateTimeButton(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiShowColoredTopLeftButton("SET DATE TIME", RED);
        uiRequestUpdate();
        changedCallbacks.A = &_showInitialButtonFunctions;
    }
}

static bool countingInitialButtonCycleCounter;
static uint32_t showInitialButtonCyclesCounter;

static void hideInitialButtonFunctions() {
    showInitialButtonCyclesCounter = 0;
    countingInitialButtonCycleCounter = false;
    uiShowTopLeftButton(NULL);
    uiShowBottomLeftButton(NULL);
    uiRequestUpdate();
}

void _showInitialButtonFunctions(const Button button, const bool buttonOn) {
    if (buttonOn) {
        showInitialButtonCyclesCounter = 0;
        changedCallbacks.A = &clickedSetDateTimeButton;
        if (!countingInitialButtonCycleCounter) {
            countingInitialButtonCycleCounter = true;
            uiShowTopLeftButton("SET DATE TIME");
            uiShowBottomLeftButton("SET BRIT NESS");
            uiRequestUpdate();
        }
    }
}

static void resetChangedCallbacks() {
    changedCallbacks.A = &_showInitialButtonFunctions;
    changedCallbacks.B = &_showInitialButtonFunctions;
    changedCallbacks.X = &_showInitialButtonFunctions;
    changedCallbacks.Y = &_showInitialButtonFunctions;
}

void uiControllerInit() {
    uiInit();
    uiSetBrightness(100);
    buttonHandlerInit();

    resetChangedCallbacks();

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

    if (countingInitialButtonCycleCounter) {
        if (cyclesToSeconds(showInitialButtonCyclesCounter) > 2.0F) {
            hideInitialButtonFunctions();
        } else {
            showInitialButtonCyclesCounter++;
        }
    }
}
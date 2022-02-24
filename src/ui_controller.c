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
    if (buttonOn) uiViewShowBottomLeftButton("B");
    else uiViewShowBottomLeftButton(NULL);
    uiViewRequestUpdate();
}

static void showA(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiViewShowTopLeftButton("A");
    } else {
        uiViewShowTopLeftButton(NULL);
    }
    uiViewRequestUpdate();
}

static void showX(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiViewShowTopRightButton("X");
    } else {
        uiViewShowTopRightButton(NULL);
    }
    uiViewRequestUpdate();
}

static void showY(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiViewShowBottomRightButton("Y");
    } else {
        uiViewShowBottomRightButton(NULL);
    }
    uiViewRequestUpdate();
}

static void clickedSetDateTimeButton(const Button button, const bool buttonOn) {
    if (buttonOn) {
        uiViewShowColoredTopLeftButton("SET DATE TIME", RED);
        uiViewRequestUpdate();
        changedCallbacks.A = &_showInitialButtonFunctions;
    }
}

static bool countingInitialButtonCycleCounter;
static uint32_t showInitialButtonCyclesCounter;

static void hideInitialButtonFunctions() {
    showInitialButtonCyclesCounter = 0;
    countingInitialButtonCycleCounter = false;
    uiViewShowTopLeftButton(NULL);
    uiViewShowBottomLeftButton(NULL);
    uiViewRequestUpdate();
}

void _showInitialButtonFunctions(const Button button, const bool buttonOn) {
    if (buttonOn) {
        showInitialButtonCyclesCounter = 0;
        changedCallbacks.A = &clickedSetDateTimeButton;
        if (!countingInitialButtonCycleCounter) {
            countingInitialButtonCycleCounter = true;
            uiViewShowTopLeftButton("SET DATE TIME");
            uiViewShowBottomLeftButton("SET BRIT NESS");
            uiViewRequestUpdate();
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
    uiViewLoop();

    if (countingInitialButtonCycleCounter) {
        if (cyclesToSeconds(showInitialButtonCyclesCounter) > 2.0F) {
            hideInitialButtonFunctions();
        } else {
            showInitialButtonCyclesCounter++;
        }
    }
}
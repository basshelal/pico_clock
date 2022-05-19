#include <stdlib.h>
#include "ui_model.h"
#include "../utils.h"
#include "../types.h"
#include "ui_view.h"
#include "../peripherals/peripherals.h"

#define DEFAULT_TIMEOUT_MILLIS 3500
#define LONG_TIMEOUT_MILLIS 5000
#define MESSAGE_BUFFER_SIZE 64

private struct {
    DateTime *dateTime;
    DateTimeChangedCallback dateTimeChangedCallback;
    bool countingMillisSinceLastPress;
    uint64_t millisSinceLastPress;
    UIActivity activity;
    uint64_t timeoutMillis;
    struct {
        int highlightIndex;
    } setTimeState;
    struct {
        char timeText[9]; // 8 chars in HH:MM:SS format + 1 for string NULL terminator
        char dateText[14]; // 13 chars in DOW DD-MMM-YY + 1 for string NULL terminator
        char batteryText[11]; // 10 chars in 100% 4.20V + 1 for string NULL terminator
        char messageText[MESSAGE_BUFFER_SIZE];
    } buffers;
} state;

#define FORMAT_MESSAGE(fmt, args...) snprintf(state.buffers.messageText, MESSAGE_BUFFER_SIZE,fmt, ##args)

private void timeChanged(const struct Time *const oldTime,
                         const struct Time *const newTime) {
    state.dateTime->time = *newTime;
    sprintf(state.buffers.timeText, "%02i:%02i:%02i",
            state.dateTime->time.hours, state.dateTime->time.minutes, state.dateTime->time.seconds);
    log("%s\n", state.buffers.timeText);
    uiView_showClock(state.buffers.timeText);

    // TODO: 19-May-2022 @basshelal: Use a callback based approach to battery updates similar to clock
    sprintf(state.buffers.batteryText, "%02.f%% %01.02fV",
            peripherals_batteryGetPercentage(),
            peripherals_batteryGetVoltage());
    log("%s\n", state.buffers.batteryText);

    uiView_showBatteryPercentage(state.buffers.batteryText);

    uiView_requestUpdate();
}

private void dateChanged(const struct Date *const oldDate,
                         const struct Date *const newDate) {
    state.dateTime->date = *newDate;
    sprintf(state.buffers.dateText, "%s %02i-%s-%02i",
            weekdayToString(state.dateTime->date.weekDay),
            state.dateTime->date.day, monthToString(state.dateTime->date.month), state.dateTime->date.year);
    log("%s\n", state.buffers.dateText);
    uiView_showDate(state.buffers.dateText);
    uiView_requestUpdate();
}


public void uiModel_init() {
    uiView_init();
    state.dateTime = calloc(sizeof(DateTime), 1);
    state.timeoutMillis = DEFAULT_TIMEOUT_MILLIS;
    peripherals_clockSetTimeChangedCallback(timeChanged);
    peripherals_clockSetDateChangedCallback(dateChanged);
}

public void uiModel_buttonPressed() {
    state.millisSinceLastPress = 0;
    state.countingMillisSinceLastPress = true;
}

public const UIActivity uiModel_getActivity() {
    return state.activity;
}

public void uiModel_setActivity(const UIActivity activity) {
    switch (activity) {
        case HIDDEN:
            state.activity = HIDDEN;
            state.countingMillisSinceLastPress = false;
            state.millisSinceLastPress = 0;
            state.timeoutMillis = DEFAULT_TIMEOUT_MILLIS;
            state.setTimeState.highlightIndex = 0;
            uiView_clearDetails();
            uiView_hideClockHighlight();
            uiView_requestUpdate();
            break;
        case DETAILS:
            state.activity = DETAILS;
            uiView_showTopLeftButton("SET BRIT");
            uiView_showBottomLeftButton("SET TIME");
            uiView_showTopRightButton("SET DATE");
            uiView_showBottomRightButton("SET ALRM");
            uiView_requestUpdate();
            break;
        case SET_BRIGHTNESS:
            state.activity = SET_BRIGHTNESS;
            uiView_showTopRightButton("+");
            uiView_showBottomRightButton("-");
            uiView_showTopLeftButton(NULL);
            uiView_showBottomLeftButton(NULL);
            FORMAT_MESSAGE("%u %%", peripherals_displayGetBrightness());
            uiView_showMessage(state.buffers.messageText);
            uiView_requestUpdate();
            break;
        case SET_TIME:
            state.activity = SET_TIME;
            state.timeoutMillis = LONG_TIMEOUT_MILLIS;
            uiView_showTopLeftButton("<");
            uiView_showBottomLeftButton(">");
            uiView_showTopRightButton("+");
            uiView_showBottomRightButton("-");
            uiView_showMessage("Set time");
            state.setTimeState.highlightIndex = -1;
            uiModel_incrementClockHighlight();
            uiView_requestUpdate();
            break;
        case SET_DATE:
            break;
        case SET_ALARM:
            break;
    }
}

public void uiModel_incrementBrightness() {
    const int backlight = peripherals_displayGetBrightness();
    if (backlight <= 95) {
        peripherals_displaySetBrightness(backlight + 5);
        FORMAT_MESSAGE("%u %%", peripherals_displayGetBrightness());
        uiView_showMessage(state.buffers.messageText);
        uiView_requestUpdate();
    }
}

public void uiModel_decrementBrightness() {
    const int backlight = peripherals_displayGetBrightness();
    if (backlight >= 5) {
        peripherals_displaySetBrightness(backlight - 5);
        FORMAT_MESSAGE("%u %%", peripherals_displayGetBrightness());
        uiView_showMessage(state.buffers.messageText);
        uiView_requestUpdate();
    }
}

public void uiModel_incrementClockHighlight() {
    if (state.setTimeState.highlightIndex < 7) {
        state.setTimeState.highlightIndex++;
        if (state.setTimeState.highlightIndex == 2 || state.setTimeState.highlightIndex == 5)
            state.setTimeState.highlightIndex++;
        uiView_showClockHighlight(state.setTimeState.highlightIndex,
                                  state.setTimeState.highlightIndex + 1, HIGHLIGHT_COLOR);
        uiView_requestUpdate();
    }
}

public void uiModel_decrementRightClockHighlight() {
    if (state.setTimeState.highlightIndex > 0) {
        state.setTimeState.highlightIndex--;
        if (state.setTimeState.highlightIndex == 2 || state.setTimeState.highlightIndex == 5)
            state.setTimeState.highlightIndex--;
        uiView_showClockHighlight(state.setTimeState.highlightIndex,
                                  state.setTimeState.highlightIndex + 1, HIGHLIGHT_COLOR);
        uiView_requestUpdate();
    }
}

public void uiModel_loop() {
    uiView_loop();
    if (state.countingMillisSinceLastPress) {
        state.millisSinceLastPress += MILLIS_PER_CYCLE_MAIN_CORE;
    }
    if (state.millisSinceLastPress > state.timeoutMillis) {
        uiModel_setActivity(HIDDEN);
    }
}

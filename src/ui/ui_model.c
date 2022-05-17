#include "ui_model.h"
#include "../utils.h"
#include "../types.h"
#include "ui_view.h"
#include "../peripherals/rtc.h"
#include "../peripherals/battery.h"

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
        char batteryText[64];
        char messageText[MESSAGE_BUFFER_SIZE];
    } buffers;
} state;

#define FORMAT_MESSAGE(fmt, args...) snprintf(state.buffers.messageText, MESSAGE_BUFFER_SIZE,fmt, ##args)

private void updateText() {
    sprintf(state.buffers.timeText, "%02i:%02i:%02i",
            state.dateTime->hours, state.dateTime->minutes, state.dateTime->seconds);
    sprintf(state.buffers.dateText, "%s %02i-%s-%02i",
            weekdayToString(state.dateTime->weekDay),
            state.dateTime->date, monthToString(state.dateTime->month), state.dateTime->year);
    sprintf(state.buffers.batteryText, "%02.f%% %02.02fV %02.fmW %02.02fmV",
            battery_getPercentage(),
            battery_getBusVoltageVolts(),
            battery_getPowerMilliWatts(),
            battery_getShuntVoltageMilliVolts());
    clear_console();
    log("%s\n", state.buffers.timeText);
    log("%s\n", state.buffers.dateText);
    log("%s\n", state.buffers.batteryText);

    uiView_showBatteryPercentage(state.buffers.batteryText);
    uiView_showClock(state.buffers.timeText);
    uiView_showDate(state.buffers.dateText);

    uiView_requestUpdate();
}

private void dateTimeChanged(const struct DateTime *const oldDateTime,
                             const struct DateTime *const newDateTime) {
    state.dateTime = newDateTime;
    updateText();
}

public void uiModel_init() {
    uiView_init();
    state.timeoutMillis = DEFAULT_TIMEOUT_MILLIS;
    rtc_init();
    if (!rtc_isRunning()) rtc_setIsRunning(true);
    rtc_setDateTimeChangedCallback(dateTimeChanged);

    battery_init();
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
            FORMAT_MESSAGE("%u %%", display_getBacklight());
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
    const int backlight = display_getBacklight();
    if (backlight <= 95) {
        display_setBacklight(backlight + 5);
        FORMAT_MESSAGE("%u %%", display_getBacklight());
        uiView_showMessage(state.buffers.messageText);
        uiView_requestUpdate();
    }
}

public void uiModel_decrementBrightness() {
    const int backlight = display_getBacklight();
    if (backlight >= 5) {
        display_setBacklight(backlight - 5);
        FORMAT_MESSAGE("%u %%", display_getBacklight());
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
    rtc_loop();
}

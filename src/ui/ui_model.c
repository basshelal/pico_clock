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
        enum {
            HOURS = 0, MINUTES = 3, SECONDS = 6
        } field;
        Time time;
    } setTimeState;
    struct {
        enum {
            WEEKDAY = 0, DAY = 4, MONTH = 7, YEAR = 11
        } field;
        uint8_t fieldLength;
        Date date;
    } setDateState;
    struct {
        char timeText[9]; // 8 chars in HH:MM:SS format + 1 for string NULL terminator
        char dateText[14]; // 13 chars in DOW DD-MMM-YY + 1 for string NULL terminator
        char batteryText[11]; // 10 chars in 100% 4.20V + 1 for string NULL terminator
        char messageText[MESSAGE_BUFFER_SIZE];
    } buffers;
} state;

#define FORMAT_MESSAGE(fmt, args...) snprintf(state.buffers.messageText, MESSAGE_BUFFER_SIZE,fmt, ##args)

private void formatTime(const Time *time, char *buffer) {
    sprintf(buffer, "%02i:%02i:%02i", time->hours, time->minutes, time->seconds);
}

private void timeChanged(const struct Time *const oldTime,
                         const struct Time *const newTime) {
    state.dateTime->time = *newTime;
    formatTime(newTime, state.buffers.timeText);
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

private void formatDate(const Date *date, char *buffer) {
    sprintf(buffer, "%s %02i-%s-%02i",
            weekdayToString(date->weekDay), date->day, monthToString(date->month), date->year);
}

private void dateChanged(const struct Date *const oldDate,
                         const struct Date *const newDate) {
    state.dateTime->date = *newDate;
    formatDate(&state.dateTime->date, state.buffers.dateText);
    log("%s\n", state.buffers.dateText);
    uiView_showDate(state.buffers.dateText);
    uiView_requestUpdate();
}

private uint8_t lastDayOfMonth(Date *date) {
    bool isLeapYear = (date->year % 4) == 0;
    switch (date->month) {
        case 1: // January
            return 31;
        case 2: // February
            return isLeapYear ? 29 : 28;
        case 3: // March
            return 31;
        case 4: // April
            return 30;
        case 5: // May
            return 31;
        case 6: // June
            return 30;
        case 7: // July
            return 31;
        case 8: // August
            return 31;
        case 9: // September
            return 30;
        case 10: // October
            return 31;
        case 11: // November
            return 30;
        case 12: // December
            return 31;
        default:
            return 0;
    }
}

private bool fixDate(Date *date) {
    if (date->day > lastDayOfMonth(date)) {
        date->day = lastDayOfMonth(date);
        return true;
    }
    return false;
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
            uiView_clearDetails();
            uiView_hideClockHighlight();
            uiView_hideDateHighlight();
            uiModel_cancelSetTime();
            uiModel_cancelSetDate();
            uiView_requestUpdate();
            break;
        case DETAILS:
            state.activity = DETAILS;
            uiView_showTopLeftButton("SET BRIT");
            uiView_showBottomLeftButton("SET TIME");
            uiView_showTopRightButton("SET DATE");
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
            state.setTimeState.field = SECONDS;
            Time currentTime;
            peripherals_clockGetTime(&currentTime);
            state.setTimeState.time = currentTime;
            peripherals_clockSetRunning(false);
            uiView_showTopLeftButton(">");
            uiView_showTopRightButton("+");
            uiView_showBottomLeftButton("x");
            uiView_showBottomRightButton("OK");
            uiView_showMessage("Set time");
            uiModel_incrementClockHighlight();
            uiView_requestUpdate();
            break;
        case SET_DATE:
            state.activity = SET_DATE;
            state.timeoutMillis = LONG_TIMEOUT_MILLIS;
            state.setDateState.field = YEAR;
            Date currentDate;
            peripherals_clockGetDate(&currentDate);
            state.setDateState.date = currentDate;
            peripherals_clockSetRunning(false);
            uiView_showTopLeftButton(">");
            uiView_showTopRightButton("+");
            uiView_showBottomLeftButton("x");
            uiView_showBottomRightButton("OK");
            uiView_showMessage("Set date");
            uiModel_incrementDateHighlight();
            uiView_requestUpdate();
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
    switch (state.setTimeState.field) {
        case HOURS:
            state.setTimeState.field = MINUTES;
            break;
        case MINUTES:
            state.setTimeState.field = SECONDS;
            break;
        case SECONDS:
            state.setTimeState.field = HOURS;
            break;
    }
    uiView_showClockHighlight(state.setTimeState.field,
                              state.setTimeState.field + 2, HIGHLIGHT_COLOR);
    uiView_requestUpdate();
}

public void uiModel_decrementClockHighlight() {
    switch (state.setTimeState.field) {
        case HOURS:
            state.setTimeState.field = SECONDS;
            break;
        case MINUTES:
            state.setTimeState.field = HOURS;
            break;
        case SECONDS:
            state.setTimeState.field = MINUTES;
            break;
    }
    uiView_showClockHighlight(state.setTimeState.field,
                              state.setTimeState.field + 2, HIGHLIGHT_COLOR);
    uiView_requestUpdate();
}

public void uiModel_incrementDateHighlight() {
    switch (state.setDateState.field) {
        case WEEKDAY:
            state.setDateState.field = DAY;
            state.setDateState.fieldLength = 2;
            break;
        case DAY:
            state.setDateState.field = MONTH;
            state.setDateState.fieldLength = 3;
            break;
        case MONTH:
            state.setDateState.field = YEAR;
            state.setDateState.fieldLength = 2;
            break;
        case YEAR:
            state.setDateState.field = WEEKDAY;
            state.setDateState.fieldLength = 3;
            break;
    }
    uiView_showDateHighlight(state.setDateState.field,
                             state.setDateState.field + state.setDateState.fieldLength, HIGHLIGHT_COLOR);
    uiView_requestUpdate();
}

public void uiModel_decrementDateHighlight() {
    switch (state.setDateState.field) {
        case WEEKDAY:
            state.setDateState.field = YEAR;
            state.setDateState.fieldLength = 2;
            break;
        case DAY:
            state.setDateState.field = WEEKDAY;
            state.setDateState.fieldLength = 3;
            break;
        case MONTH:
            state.setDateState.field = DAY;
            state.setDateState.fieldLength = 2;
            break;
        case YEAR:
            state.setDateState.field = MONTH;
            state.setDateState.fieldLength = 3;
            break;
    }
    uiView_showDateHighlight(state.setDateState.field,
                             state.setDateState.field + state.setDateState.fieldLength, HIGHLIGHT_COLOR);
    uiView_requestUpdate();
}

public void uiModel_incrementSetTime() {
    switch (state.setTimeState.field) {
        case HOURS:
            if (state.setTimeState.time.hours < 23) state.setTimeState.time.hours++;
            else state.setTimeState.time.hours = 0;
            break;
        case MINUTES:
            if (state.setTimeState.time.minutes < 59) state.setTimeState.time.minutes++;
            else state.setTimeState.time.minutes = 0;
            break;
        case SECONDS:
            if (state.setTimeState.time.seconds < 59) state.setTimeState.time.seconds++;
            else state.setTimeState.time.seconds = 0;
            break;
    }
    formatTime(&state.setTimeState.time, state.buffers.timeText);
    uiView_showClock(state.buffers.timeText);
    uiView_requestUpdate();
}

public void uiModel_decrementSetTime() {
    switch (state.setTimeState.field) {
        case HOURS:
            if (state.setTimeState.time.hours > 0) state.setTimeState.time.hours--;
            else state.setTimeState.time.hours = 23;
            break;
        case MINUTES:
            if (state.setTimeState.time.minutes > 0) state.setTimeState.time.minutes--;
            else state.setTimeState.time.minutes = 59;
            break;
        case SECONDS:
            if (state.setTimeState.time.seconds > 0) state.setTimeState.time.seconds--;
            else state.setTimeState.time.seconds = 59;
            break;
    }
    formatTime(&state.setTimeState.time, state.buffers.timeText);
    uiView_showClock(state.buffers.timeText);
    uiView_requestUpdate();
}

public void uiModel_acceptSetTime() {
    peripherals_clockSetTime(&state.setTimeState.time);
    peripherals_clockSetRunning(true);
    formatTime(&state.setTimeState.time, state.buffers.timeText);
    uiView_showClock(state.buffers.timeText);
    uiView_forceUpdate();
    uiModel_setActivity(HIDDEN);
}

public void uiModel_cancelSetTime() {
    peripherals_clockSetRunning(true);
    uiView_forceUpdate();
    if (state.activity != HIDDEN) uiModel_setActivity(HIDDEN);
}

public void uiModel_incrementSetDate() {
    switch (state.setDateState.field) {
        case WEEKDAY:
            if (state.setDateState.date.weekDay < SUNDAY) state.setDateState.date.weekDay++;
            else state.setDateState.date.weekDay = MONDAY;
            break;
        case DAY:
            if (state.setDateState.date.day < lastDayOfMonth(&state.setDateState.date))
                state.setDateState.date.day++;
            else state.setDateState.date.day = 1;
            break;
        case MONTH:
            if (state.setDateState.date.month < 12) state.setDateState.date.month++;
            else state.setDateState.date.month = 1;
            break;
        case YEAR:
            if (state.setDateState.date.year < 99) state.setDateState.date.year++;
            else state.setDateState.date.year = 0;
            break;
    }
    fixDate(&state.setDateState.date);
    formatDate(&state.setDateState.date, state.buffers.dateText);
    uiView_showDate(state.buffers.dateText);
    uiView_requestUpdate();
}

public void uiModel_decrementSetDate() {
    switch (state.setDateState.field) {
        case WEEKDAY:
            if (state.setDateState.date.weekDay > MONDAY) state.setDateState.date.weekDay--;
            else state.setDateState.date.weekDay = SUNDAY;
            break;
        case DAY:
            if (state.setDateState.date.day > 1) state.setDateState.date.day--;
            else state.setDateState.date.day = lastDayOfMonth(&state.setDateState.date);
            break;
        case MONTH:
            if (state.setDateState.date.month > 0) state.setDateState.date.month--;
            else state.setDateState.date.month = 11;
            break;
        case YEAR:
            if (state.setDateState.date.year > 0) state.setDateState.date.year--;
            else state.setDateState.date.year = 99;
            break;
    }
    fixDate(&state.setDateState.date);
    formatDate(&state.setDateState.date, state.buffers.dateText);
    uiView_showDate(state.buffers.dateText);
    uiView_requestUpdate();
}

public void uiModel_acceptSetDate() {
    peripherals_clockSetDate(&state.setDateState.date);
    peripherals_clockSetRunning(true);
    formatDate(&state.setDateState.date, state.buffers.dateText);
    uiView_showDate(state.buffers.dateText);
    uiView_forceUpdate();
    uiModel_setActivity(HIDDEN);
}

public void uiModel_cancelSetDate() {
    peripherals_clockSetRunning(true);
    Date date;
    peripherals_clockGetDate(&date);
    formatDate(&date, state.buffers.dateText);
    uiView_showDate(state.buffers.dateText);
    uiView_forceUpdate();
    if (state.activity != HIDDEN) uiModel_setActivity(HIDDEN);
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

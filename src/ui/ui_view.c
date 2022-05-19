#include "ui_view.h"
#include "../utils.h"
#include <stdlib.h>
#include <pico/multicore.h>
#include <stdio.h>

#include "../peripherals/display.h"

private const uint8_t outsideMargin = 10;
private const uint8_t charWidth = 6;
private const uint8_t charHeight = 8;

private const uint8_t batteryTextScale = 1;
private Rectangle batteryRectangle = {
        .x=outsideMargin,
        .y=outsideMargin,
        .w=4 * charWidth * batteryTextScale,
        .h=batteryTextScale * charHeight
};

private const uint8_t clockTextScale = 6;
private Rectangle clockRect = {
        .x=outsideMargin * clockTextScale,
        .y=outsideMargin * 6,
        .w=6 * charWidth * clockTextScale,
        .h=1 * charHeight * clockTextScale
};
private const char *clockLastText;
private struct {
    Rectangle rectangle;
    bool hasHighlight;
    int fromIndex;
    int toIndex;
    Color color;
    char *buffer;
} clockHighlightState;

private const uint8_t dateTextScale = 2;
private Rectangle dateRect = {
        .x=outsideMargin * clockTextScale,
        .y=outsideMargin * 14,
        .w=12 * charWidth * dateTextScale,
        .h=1 * charHeight * dateTextScale};

private const uint8_t messageTextScale = 2;
private Rectangle messageRect = {
        .x=outsideMargin * clockTextScale,
        .y=outsideMargin * 18,
        .w=12 * charWidth * messageTextScale,
        .h=3 * charHeight * messageTextScale};

private const uint8_t buttonTextScale = 2;

private const int32_t leftX = 0;
private const int32_t rightX = DISPLAY_WIDTH - (4 * charWidth * buttonTextScale);
private const int32_t topY = DISPLAY_HEIGHT / 6;
private const int32_t bottomY = 4 * (DISPLAY_HEIGHT / 6);
private const int32_t width = 4 * charWidth * buttonTextScale;
private const int32_t height = 2 * charHeight * buttonTextScale;

private const int wrapWidth = 4 * charWidth * buttonTextScale;

private Rectangle topLeftButtonRect = {
        .x=leftX,
        .y=topY,
        .w=width,
        .h=height
};

private Rectangle topRightButtonRect = {
        .x=rightX,
        .y=topY,
        .w=width,
        .h=height
};

private Rectangle bottomLeftButtonRect = {
        .x=leftX,
        .y=bottomY,
        .w=width,
        .h=height
};

private Rectangle bottomRightButtonRect = {
        .x=rightX,
        .y=bottomY,
        .w=width,
        .h=height
};

private inline void uiView_setRectangle(const Rectangle rectangle, Color color) {
    display_setColor(color);
    display_setRectangle(rectangle);
    display_setColor(WHITE);
}

private inline void uiView_clearRectangle(const Rectangle rectangle) {
    uiView_setRectangle(rectangle, BLACK);
}

private inline void uiView_loopUICore() {
    int updateRequestCount = 0;
    while (multicore_fifo_rvalid()) {
        uint32_t data;
        multicore_fifo_pop_timeout_us(10000, &data);
        if (data == REQUEST_UPDATE) updateRequestCount++;
    }
    if (updateRequestCount > 0) {
        uiView_forceUpdate();
        printf("Update requests: %i\n", updateRequestCount);
    }
}

private void uiView_launchUICore() {
    while (true) {
        uiView_loopUICore();
        sleep_ms(MILLIS_PER_CYCLE_UI_CORE);
    }
}

public const Color WHITE = {.r = 255, .g = 255, .b = 255};
public const Color BLACK = {.r = 0, .g = 0, .b = 0};
public const Color RED = {.r = 255, .g = 0, .b = 0};
public const Color GREEN = {.r = 0, .g = 255, .b = 0};
public const Color BLUE = {.r = 0, .g = 0, .b = 255};
public const Color YELLOW = {.r = 255, .g = 255, .b = 0};
public const Color HIGHLIGHT_COLOR = {.r = 200, .g = 20, .b = 20};

#define TEXT_COLOR WHITE

public void uiView_init() {
    uiView_clearAll();

    multicore_launch_core1(&uiView_launchUICore);
    uiView_requestUpdate();
    uiView_setBrightness(100);

    clockHighlightState.buffer = (char *) calloc(sizeof(char), 8);
}

public void uiView_clearAll() {
    display_clear();
}

public void uiView_clearDetails() {
    uiView_showTopLeftButton(NULL);
    uiView_showBottomLeftButton(NULL);
    uiView_showTopRightButton(NULL);
    uiView_showBottomRightButton(NULL);
    uiView_showMessage(NULL);
}

public void uiView_showBatteryPercentage(const char *text) {
    uiView_clearRectangle(batteryRectangle);
    if (text != NULL) {
        batteryRectangle.w = display_getStringWidth(text, batteryTextScale);
        display_setColor(TEXT_COLOR);
        display_setText(text, batteryRectangle.x, batteryRectangle.y,
                        DISPLAY_WIDTH - outsideMargin, batteryTextScale);
    }
}

public void uiView_showClockHighlight(const int fromIndex, const int toIndex, const Color color) {
    clockHighlightState.hasHighlight = true;
    clockHighlightState.fromIndex = fromIndex;
    clockHighlightState.toIndex = toIndex;
    clockHighlightState.color = color;
    uiView_showClock(clockLastText);
}

public void uiView_hideClockHighlight() {
    clockHighlightState.hasHighlight = false;
    uiView_showClock(clockLastText);
}

public void uiView_showClock(const char *text) {
    clockLastText = text;
    uiView_clearRectangle(clockRect);
    if (text != NULL) {
        clockRect.w = display_getStringWidth(text, clockTextScale);
        clockRect.x = (DISPLAY_WIDTH / 2) - (clockRect.w / 2);
        if (clockHighlightState.hasHighlight) {
            clockHighlightState.rectangle.y = clockRect.y;
            clockHighlightState.rectangle.h = clockRect.h;

            substring(text, 0, clockHighlightState.fromIndex, clockHighlightState.buffer);
            int leftWidth = display_getStringWidth(clockHighlightState.buffer, clockTextScale);
            substring(text, clockHighlightState.toIndex, 8, clockHighlightState.buffer);
            int rightWidth = display_getStringWidth(clockHighlightState.buffer, clockTextScale);
            clockHighlightState.rectangle.x = clockRect.x + leftWidth;
            clockHighlightState.rectangle.w = clockRect.w - (rightWidth + leftWidth);

            display_setColor(clockHighlightState.color);
            display_setRectangle(clockHighlightState.rectangle);
        }
        display_setColor(TEXT_COLOR);
        display_setText(text, clockRect.x, clockRect.y,
                        DISPLAY_WIDTH - outsideMargin, clockTextScale);
    }
}

public void uiView_showDate(const char *text) {
    uiView_clearRectangle(dateRect);
    if (text) {
        dateRect.w = display_getStringWidth(text, dateTextScale);
        dateRect.x = (DISPLAY_WIDTH / 2) - (dateRect.w / 2);
        display_setColor(TEXT_COLOR);
        display_setText(text, dateRect.x, dateRect.y,
                        DISPLAY_WIDTH - outsideMargin, dateTextScale);
    }
}

public void uiView_showMessage(const char *text) {
    uiView_clearRectangle(messageRect);
    if (text != NULL) {
        const int wrapWidth = DISPLAY_WIDTH - (2 * 4 * charWidth * buttonTextScale);
        const int fullLineWidth = display_getStringWidth(text, messageTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        messageRect.w = lineWidth;
        messageRect.x = (DISPLAY_WIDTH / 2) - (messageRect.w / 2);
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        messageRect.h = (lines * charHeight * messageTextScale) + (lineBreaks * messageTextScale);
        display_setColor(TEXT_COLOR);
        display_setText(text, messageRect.x, messageRect.y,
                        wrapWidth, messageTextScale);
    }
}

public void uiView_showTopLeftButtonColored(const char *text, const Color textColor) {
    uiView_clearRectangle(topLeftButtonRect);
    if (text != NULL) {
        const int fullLineWidth = display_getStringWidth(text, buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        topLeftButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        topLeftButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        display_setColor(textColor);
        display_setText(text, topLeftButtonRect.x, topLeftButtonRect.y,
                        wrapWidth, buttonTextScale);
        display_setColor(WHITE);
    }
}

public void uiView_showTopLeftButton(const char *text) {
    uiView_showTopLeftButtonColored(text, TEXT_COLOR);
}

public void uiView_showBottomLeftButton(const char *text) {
    uiView_clearRectangle(bottomLeftButtonRect);
    if (text != NULL) {
        const int fullLineWidth = display_getStringWidth(text, buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        bottomLeftButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        bottomLeftButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        display_setColor(TEXT_COLOR);
        display_setText(text, bottomLeftButtonRect.x, bottomLeftButtonRect.y,
                        wrapWidth, buttonTextScale);
    }
}

public void uiView_showTopRightButton(const char *text) {
    uiView_clearRectangle(topRightButtonRect);
    if (text != NULL) {
        const int fullLineWidth = display_getStringWidth(text, buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        topRightButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        topRightButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        topRightButtonRect.x = lines <= 1 ? (DISPLAY_WIDTH - lineWidth) : rightX;
        display_setColor(TEXT_COLOR);
        display_setText(text, topRightButtonRect.x, topRightButtonRect.y,
                        wrapWidth, buttonTextScale);
    }
}

public void uiView_showBottomRightButton(const char *text) {
    uiView_clearRectangle(bottomRightButtonRect);
    if (text != NULL) {
        const int fullLineWidth = display_getStringWidth(text, buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        bottomRightButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        bottomRightButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        bottomRightButtonRect.x = lines <= 1 ? (DISPLAY_WIDTH - lineWidth) : rightX;
        display_setColor(TEXT_COLOR);
        display_setText(text, bottomRightButtonRect.x, bottomRightButtonRect.y,
                        wrapWidth, buttonTextScale);
    }
}

public void uiView_setBrightness(const uint8_t percentage) {
    display_setBacklight(percentage);
}

public void uiView_requestUpdate() {
    multicore_fifo_push_timeout_us(REQUEST_UPDATE, 5000);
}

public void uiView_forceUpdate() {
    display_update();
}

public void uiView_loop() {

}
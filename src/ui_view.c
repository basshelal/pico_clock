#include "ui_view.h"

const Color WHITE = {.r = 255, .g = 255, .b = 255};
const Color BLACK = {.r = 0, .g = 0, .b = 0};
const Color RED = {.r = 255, .g = 0, .b = 0};
const Color GREEN = {.r = 0, .g = 255, .b = 0};
const Color BLUE = {.r = 0, .g = 0, .b = 255};
const Color YELLOW = {.r = 255, .g = 255, .b = 0};

static uint16_t *buffer;

static const uint8_t outsideMargin = 10;
static const uint8_t charWidth = 6;
static const uint8_t charHeight = 8;
static const uint8_t batteryTextScale = 1;

static Rectangle batteryRectangle = {
        .x=outsideMargin,
        .y=outsideMargin,
        .w=4 * charWidth * batteryTextScale,
        .h=batteryTextScale * charHeight
};

static const uint8_t clockTextScale = 6;

static Rectangle clockRect = {
        .x=outsideMargin * clockTextScale,
        .y=outsideMargin * 6,
        .w=6 * charWidth * clockTextScale,
        .h=1 * charHeight * clockTextScale
};

static const uint8_t dateTextScale = 2;

static Rectangle dateRect = {
        .x=outsideMargin * clockTextScale,
        .y=outsideMargin * 14,
        .w=12 * charWidth * dateTextScale,
        .h=1 * charHeight * dateTextScale};

static const uint8_t messageTextScale = 2;

static Rectangle messageRect = {
        .x=outsideMargin * clockTextScale,
        .y=outsideMargin * 18,
        .w=12 * charWidth * messageTextScale,
        .h=3 * charHeight * messageTextScale};

static const uint8_t buttonTextScale = 2;

static const int32_t leftX = 0;
static const int32_t rightX = DISPLAY_WIDTH - (4 * charWidth * buttonTextScale);
static const int32_t topY = DISPLAY_HEIGHT / 6;
static const int32_t bottomY = 4 * (DISPLAY_HEIGHT / 6);
static const int32_t width = 4 * charWidth * buttonTextScale;
static const int32_t height = 2 * charHeight * buttonTextScale;

static const int wrapWidth = 4 * charWidth * buttonTextScale;

static Rectangle topLeftButtonRect = {
        .x=leftX,
        .y=topY,
        .w=width,
        .h=height
};

static Rectangle topRightButtonRect = {
        .x=rightX,
        .y=topY,
        .w=width,
        .h=height
};

static Rectangle bottomLeftButtonRect = {
        .x=leftX,
        .y=bottomY,
        .w=width,
        .h=height
};

static Rectangle bottomRightButtonRect = {
        .x=rightX,
        .y=bottomY,
        .w=width,
        .h=height
};

void uiViewClear() {
    displayClear();
}

static inline void uiSetRectangle(const Rectangle rectangle, Color color) {
    displaySetColor(color);
    displaySetRectangle(rectangle);
    displaySetColor(WHITE);
}

static inline void uiClearRectangle(const Rectangle rectangle) {
    uiSetRectangle(rectangle, BLACK);
}

static inline void loopUICore() {
    int updateRequestCount = 0;
    while (multicore_fifo_rvalid()) {
        uint32_t data;
        multicore_fifo_pop_timeout_us(10000, &data);
        if (data == REQUEST_UPDATE) updateRequestCount++;
    }
    if (updateRequestCount > 0) {
        uiViewForceUpdate();
        printf("Update requests: %i\n", updateRequestCount);
    }
}

static void launchUICore() {
    while (true) {
        loopUICore();
        sleep_ms(UI_CORE_CYCLE);
    }
}

void uiViewInit() {
    buffer = (uint16_t *) malloc(DISPLAY_AREA * sizeof(uint16_t));
    displayInit(buffer);
    displaySetColor(WHITE);
    uiViewClear();

    multicore_launch_core1(&launchUICore);
    uiViewRequestUpdate();
    uiViewSetBrightness(100);
}

void uiViewShowBatteryPercentage(const char *text) {
    uiClearRectangle(batteryRectangle);
    if (text != NULL) {
        batteryRectangle.w = (displayGetStringWidth(text) * batteryTextScale);
        displaySetText(text, batteryRectangle.x, batteryRectangle.y,
                       DISPLAY_WIDTH - outsideMargin, batteryTextScale);
    }
}

void uiViewShowClock(const char *text) {
    uiClearRectangle(clockRect);
    if (text != NULL) {
        clockRect.w = (displayGetStringWidth(text) * clockTextScale);
        clockRect.x = (DISPLAY_WIDTH / 2) - (clockRect.w / 2);
        displaySetText(text, clockRect.x, clockRect.y,
                       DISPLAY_WIDTH - outsideMargin, clockTextScale);
    }
}

void uiViewShowDate(const char *text) {
    uiClearRectangle(dateRect);
    if (text) {
        dateRect.w = (displayGetStringWidth(text) * dateTextScale);
        dateRect.x = (DISPLAY_WIDTH / 2) - (dateRect.w / 2);
        displaySetText(text, dateRect.x, dateRect.y,
                       DISPLAY_WIDTH - outsideMargin, dateTextScale);
    }
}

void uiViewShowMessage(const char *text) {
    uiClearRectangle(messageRect);
    if (text != NULL) {
        const int wrapWidth = DISPLAY_WIDTH - (2 * 4 * charWidth * buttonTextScale);
        const int fullLineWidth = (displayGetStringWidth(text) * messageTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        messageRect.w = lineWidth;
        messageRect.x = (DISPLAY_WIDTH / 2) - (messageRect.w / 2);
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        messageRect.h = (lines * charHeight * messageTextScale) + (lineBreaks * messageTextScale);
        displaySetText(text, messageRect.x, messageRect.y,
                       wrapWidth, messageTextScale);
    }
}

void uiViewShowColoredTopLeftButton(const char *text, const Color textColor) {
    uiClearRectangle(topLeftButtonRect);
    if (text != NULL) {
        const int fullLineWidth = (displayGetStringWidth(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        topLeftButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        topLeftButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        displaySetColor(textColor);
        displaySetText(text, topLeftButtonRect.x, topLeftButtonRect.y,
                       wrapWidth, buttonTextScale);
        displaySetColor(WHITE);
    }
}

void uiViewShowTopLeftButton(const char *text) {
    uiViewShowColoredTopLeftButton(text, WHITE);
}

void uiViewShowBottomLeftButton(const char *text) {
    uiClearRectangle(bottomLeftButtonRect);
    if (text != NULL) {
        const int fullLineWidth = (displayGetStringWidth(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        bottomLeftButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        bottomLeftButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        displaySetText(text, bottomLeftButtonRect.x, bottomLeftButtonRect.y,
                       wrapWidth, buttonTextScale);
    }
}

void uiViewShowTopRightButton(const char *text) {
    uiClearRectangle(topRightButtonRect);
    if (text != NULL) {
        const int fullLineWidth = (displayGetStringWidth(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        topRightButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        topRightButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        topRightButtonRect.x = lines <= 1 ? (DISPLAY_WIDTH - lineWidth) : rightX;
        displaySetText(text, topRightButtonRect.x, topRightButtonRect.y,
                       wrapWidth, buttonTextScale);
    }
}

void uiViewShowBottomRightButton(const char *text) {
    uiClearRectangle(bottomRightButtonRect);
    if (text != NULL) {
        const int fullLineWidth = (displayGetStringWidth(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        bottomRightButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        bottomRightButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        bottomRightButtonRect.x = lines <= 1 ? (DISPLAY_WIDTH - lineWidth) : rightX;
        displaySetText(text, bottomRightButtonRect.x, bottomRightButtonRect.y,
                       wrapWidth, buttonTextScale);
    }
}

void uiViewSetBrightness(const uint8_t percentage) {
    displaySetBacklight(percentage);
}

void uiViewRequestUpdate() {
    multicore_fifo_push_timeout_us(REQUEST_UPDATE, 5000);
}

void uiViewForceUpdate() {
    displayUpdate();
}

void uiViewLoop() {

}
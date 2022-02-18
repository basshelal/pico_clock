#include "ui.h"

static Color color(uint8_t r, uint8_t g, uint8_t b) { return (Color) {.r=r, .g=g, .b = b}; }

static Color WHITE = {.r = 255, .g = 255, .b = 255};
static Color BLACK = {.r = 0, .g = 0, .b = 0};
static Color GREY = {.r = 127, .g = 127, .b = 127};
static Color RED = {.r = 255, .g = 0, .b = 0};
static Color GREEN = {.r = 0, .g = 255, .b = 0};
static Color BLUE = {.r = 0, .g = 0, .b = 255};
static Color YELLOW = {.r = 255, .g = 255, .b = 0};
static Color CYAN = {.r = 0, .g = 255, .b = 255};
static Color MAGENTA = {.r = 255, .g = 0, .b = 255};
static Color PINK = {.r = 255, .g = 0, .b = 127};
static Color LIME = {.r = 127, .g = 255, .b = 0};
static Color VIOLET = {.r = 127, .g = 0, .b = 255};
static Color ORANGE = {.r = 255, .g = 127, .b = 0};

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


void uiClearAll() {
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

void uiInit() {
    buffer = (uint16_t *) malloc(DISPLAY_AREA * sizeof(uint16_t));
    displayInit(buffer);
    displaySetColor(WHITE);
}

void uiShowBatteryPercentage(const char *text) {
    uiClearRectangle(batteryRectangle);
    if (text != NULL) {
        batteryRectangle.w = (displayGetStringWidth(text) * batteryTextScale);
        displaySetText(text, batteryRectangle.x, batteryRectangle.y,
                       DISPLAY_WIDTH - outsideMargin, batteryTextScale);
    }
}

void uiShowClock(const char *text) {
    uiClearRectangle(clockRect);
    if (text != NULL) {
        clockRect.w = (displayGetStringWidth(text) * clockTextScale);
        clockRect.x = (DISPLAY_WIDTH / 2) - (clockRect.w / 2);
        displaySetText(text, clockRect.x, clockRect.y,
                       DISPLAY_WIDTH - outsideMargin, clockTextScale);
    }
}

void uiShowDate(const char *text) {
    uiClearRectangle(dateRect);
    if (text) {
        dateRect.w = (displayGetStringWidth(text) * dateTextScale);
        dateRect.x = (DISPLAY_WIDTH / 2) - (dateRect.w / 2);
        displaySetText(text, dateRect.x, dateRect.y,
                       DISPLAY_WIDTH - outsideMargin, dateTextScale);
    }
}

void uiShowMessage(const char *text) {
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

void uiShowTopLeftButton(const char *text) {
    uiClearRectangle(topLeftButtonRect);
    if (text != NULL) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
        const int fullLineWidth = (displayGetStringWidth(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        topLeftButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        topLeftButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        displaySetText(text, topLeftButtonRect.x, topLeftButtonRect.y,
                       wrapWidth, buttonTextScale);
    }
}

void uiShowBottomLeftButton(const char *text) {
    uiClearRectangle(bottomLeftButtonRect);
    if (text != NULL) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
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

void uiShowTopRightButton(const char *text) {
    uiClearRectangle(topRightButtonRect);
    if (text != NULL) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
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

void uiShowBottomRightButton(const char *text) {
    uiClearRectangle(bottomRightButtonRect);
    if (text != NULL) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
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

void uiSetBrightness(const uint8_t percentage) {
    displaySetBacklight(percentage);
}

void uiUpdate() {
    displayUpdate();
    log("UI update!\n");
}

void uiLoop() {

}
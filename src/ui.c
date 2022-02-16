#include "ui.h"

static Color color(uint8_t r, uint8_t g, uint8_t b) { return (Color) {.r=r, .g=g, .b = b}; }

static Color WHITE = (Color) {.r = 255, .g = 255, .b = 255};
static Color BLACK = (Color) {.r = 0, .g = 0, .b = 0};
static Color GREY = (Color) {.r = 127, .g = 127, .b = 127};
static Color RED = (Color) {.r = 255, .g = 0, .b = 0};
static Color GREEN = (Color) {.r = 0, .g = 255, .b = 0};
static Color BLUE = (Color) {.r = 0, .g = 0, .b = 255};
static Color YELLOW = (Color) {.r = 255, .g = 255, .b = 0};
static Color CYAN = (Color) {.r = 0, .g = 255, .b = 255};
static Color MAGENTA = (Color) {.r = 255, .g = 0, .b = 255};
static Color PINK = (Color) {.r = 255, .g = 0, .b = 127};
static Color LIME = (Color) {.r = 127, .g = 255, .b = 0};
static Color VIOLET = (Color) {.r = 127, .g = 0, .b = 255};
static Color ORANGE = (Color) {.r = 255, .g = 127, .b = 0};

static uint16_t *buffer;

static const int outsideMargin = 10;
static const int charWidth = 6;
static const int charHeight = 8;
static const int batteryTextScale = 1;

static Rectangle batteryRectangle = {
        .x=outsideMargin,
        .y=outsideMargin,
        .w=4 * charWidth * batteryTextScale,
        .h=batteryTextScale * charHeight
};

static const int clockTextScale = 6;

static Rectangle clockRect = {
        .x=outsideMargin * clockTextScale,
        .y=outsideMargin * 6,
        .w=6 * charWidth * clockTextScale,
        .h=1 * charHeight * clockTextScale
};

static const int dateTextScale = 2;

static Rectangle dateRect = {
        .x=outsideMargin * clockTextScale,
        .y=outsideMargin * 14,
        .w=12 * charWidth * dateTextScale,
        .h=1 * charHeight * dateTextScale};

static const int messageTextScale = 2;

static Rectangle messageRect = {
        .x=outsideMargin * clockTextScale,
        .y=outsideMargin * 18,
        .w=12 * charWidth * messageTextScale,
        .h=3 * charHeight * messageTextScale};

static const int buttonTextScale = 2;

static Rectangle topLeftButtonRect = {
        .x=0,
        .y=DISPLAY_HEIGHT / 6,
        .w=4 * charWidth * buttonTextScale,
        .h=2 * charHeight * buttonTextScale
};

static Rectangle topRightButtonRect = {
        .x=DISPLAY_WIDTH - (4 * charWidth * buttonTextScale),
        .y=DISPLAY_HEIGHT / 6,
        .w=4 * charWidth * buttonTextScale,
        .h=2 * charHeight * buttonTextScale
};

static Rectangle bottomLeftButtonRect = {
        .x=0,
        .y=4 * (DISPLAY_HEIGHT / 6),
        .w=4 * charWidth * buttonTextScale,
        .h=2 * charHeight * buttonTextScale
};

static Rectangle bottomRightButtonRect = {
        .x=DISPLAY_WIDTH - (4 * charWidth * buttonTextScale),
        .y=4 * (DISPLAY_HEIGHT / 6),
        .w=4 * charWidth * buttonTextScale,
        .h=2 * charHeight * buttonTextScale
};


void uiClearAll() {
    displayClear();
}

void uiSetRectangle(const Rectangle rectangle, Color color) {
    displaySetColor(color);
    displaySetRectangle(rectangle);
    displaySetColor(WHITE);
}

void uiClearRectangle(const Rectangle rectangle) {
    uiSetRectangle(rectangle, BLACK);
}

void uiInit() {
    buffer = (uint16_t *) malloc(DISPLAY_AREA * sizeof(uint16_t));
    displayInit(buffer);
    displaySetColor(WHITE);
}

void uiShowBatteryPercentage(const char *text) {
    uiSetRectangle(batteryRectangle, BLACK);
    if (text != NULL) {
        batteryRectangle.w = (displayGetStringWidth(text) * batteryTextScale);
        displaySetText(text, batteryRectangle.x, batteryRectangle.y,
                       DISPLAY_WIDTH - outsideMargin, batteryTextScale);
    }
}

void uiShowClock(const char *text) {
    uiSetRectangle(clockRect, BLACK);
    if (text != NULL) {
        clockRect.w = (displayGetStringWidth(text) * clockTextScale);
        clockRect.x = (DISPLAY_WIDTH / 2) - (clockRect.w / 2);
        displaySetText(text, clockRect.x, clockRect.y,
                       DISPLAY_WIDTH - outsideMargin, clockTextScale);
    }
}

void uiShowDate(const char *text) {
    uiSetRectangle(dateRect, BLACK);
    if (text) {
        dateRect.w = (displayGetStringWidth(text) * dateTextScale);
        dateRect.x = (DISPLAY_WIDTH / 2) - (dateRect.w / 2);
        displaySetText(text, dateRect.x, dateRect.y,
                       DISPLAY_WIDTH - outsideMargin, dateTextScale);
    }
}

void uiShowMessage(const char *text) {
    uiSetRectangle(messageRect, BLACK);
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
    uiSetRectangle(topLeftButtonRect, BLACK);
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
    uiSetRectangle(bottomLeftButtonRect, BLACK);
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
    uiSetRectangle(topRightButtonRect, BLACK);
    if (text != NULL) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
        const int fullLineWidth = (displayGetStringWidth(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        topRightButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        topRightButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
        displaySetText(text, topRightButtonRect.x, topRightButtonRect.y,
                       wrapWidth, buttonTextScale);
    }
}

void uiShowBottomRightButton(const char *text) {
    uiSetRectangle(bottomRightButtonRect, BLACK);
    if (text != NULL) {
        const int wrapWidth = 4 * charWidth * buttonTextScale;
        const int fullLineWidth = (displayGetStringWidth(text) * buttonTextScale);
        int lineWidth = fullLineWidth;
        if (lineWidth > wrapWidth) lineWidth = wrapWidth;
        bottomRightButtonRect.w = lineWidth;
        const uint8_t lines = (fullLineWidth + (wrapWidth - 1)) / wrapWidth;
        const uint8_t lineBreaks = lines - 1;
        bottomRightButtonRect.h = (lines * charHeight * buttonTextScale) + (lineBreaks * buttonTextScale);
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

#ifndef PICO_CLOCK_DISPLAY_H
#define PICO_CLOCK_DISPLAY_H

#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DISPLAY_WIDTH 320

#define DISPLAY_HEIGHT 240

#define DISPLAY_AREA 76800 // 320 * 240

typedef struct Rectangle {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} Rectangle;

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

void displayInit(uint16_t *buffer);

void displayClear();

void displaySetText(const char *text, int32_t x, int32_t y, int32_t wrap, uint8_t scale);

void displayUpdate();

void displaySetBacklight(const uint8_t percentage);

void displaySetColor(const Color color);

void displaySetRectangle(const Rectangle rectangle);

uint8_t displayGetCharWidth(const char c);

int displayGetStringWidth(const char *string);

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_DISPLAY_H

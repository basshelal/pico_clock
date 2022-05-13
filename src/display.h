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

void display_init(uint16_t *const buffer);

void display_clear();

void display_update();

void display_setColor(const Color color);

void display_setText(const char *text, const int32_t x, const int32_t y, const int32_t wrap, const uint8_t scale);

void display_setRectangle(const Rectangle rectangle);

void display_setBacklight(const uint8_t percentage);

int display_getStringWidth(const char *string, const int scale);

Color display_getRandomColor();

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_DISPLAY_H

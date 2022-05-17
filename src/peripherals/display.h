#ifndef PICO_CLOCK_DISPLAY_H
#define PICO_CLOCK_DISPLAY_H

#include "../types.h"
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DISPLAY_WIDTH 320

#define DISPLAY_HEIGHT 240

#define DISPLAY_AREA (DISPLAY_WIDTH * DISPLAY_HEIGHT)

extern void display_init(uint16_t *const buffer);

extern void display_clear();

extern void display_update();

extern void display_setColor(const Color color);

extern void
display_setText(const char *text, const int32_t x, const int32_t y, const int32_t wrap, const uint8_t scale);

extern void display_setRectangle(const Rectangle rectangle);

extern void display_setBacklight(const uint8_t percentage);

extern uint8_t display_getBacklight();

extern int display_getStringWidth(const char *string, const int scale);

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_DISPLAY_H

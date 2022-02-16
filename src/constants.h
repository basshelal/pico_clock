#ifndef PICO_CLOCK_CONSTANTS_H
#define PICO_CLOCK_CONSTANTS_H

#include "hardware/spi.h"

// set to 0 for release, used only for printing information when in debug mode
#define DEBUG 1

#define ON_LIGHT_PIN PICO_DEFAULT_LED_PIN

#define KHz * 1000
#define MHz * 1000000

// RTC
#define RTC_MISO_PIN 12
#define RTC_MOSI_PIN 11
#define RTC_SCK_PIN 14
#define RTC_CS_PIN 13
#define RTC_SPI spi1

// Screen
#define SCREEN_LCD_TF 21
#define SCREEN_BACKLIGHT 20
#define SCREEN_LCD_MOSI 19
#define SCREEN_LCD_SCLK 18
#define SCREEN_LCD_CS 17
#define SCREEN_LCD_DC 16

#define A_BUTTON_PIN 0
#define B_BUTTON_PIN 1
#define X_BUTTON_PIN 2
#define Y_BUTTON_PIN 3
#define SCREEN_LED_PIN 4

#endif //PICO_CLOCK_CONSTANTS_H

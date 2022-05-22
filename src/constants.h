#ifndef PICO_CLOCK_CONSTANTS_H
#define PICO_CLOCK_CONSTANTS_H

#include "hardware/spi.h"
#include "hardware/i2c.h"

#define DEBUG true

#define ON_LIGHT_PIN PICO_DEFAULT_LED_PIN

// Battery
#define BATTERY_SCLK_PIN 7
#define BATTERY_SDA_PIN 6
#define BATTERY_I2C i2c1
#define BATTERY_BAUD_RATE (400 * 1000)

// RTC
#define RTC_SCLK_PIN 5
#define RTC_SDA_PIN 4
#define RTC_I2C i2c0
#define RTC_BAUD_RATE (400 * 1000)

// Screen
#define SCREEN_LCD_TF 21
#define SCREEN_BACKLIGHT 20
#define SCREEN_LCD_MOSI 19
#define SCREEN_LCD_SCLK 18
#define SCREEN_LCD_CS 17
#define SCREEN_LCD_DC 16

#define SCREEN_LED_R 6
#define SCREEN_LED_G 7
#define SCREEN_LED_B 8

#define A_BUTTON_PIN 12
#define B_BUTTON_PIN 13
#define X_BUTTON_PIN 14
#define Y_BUTTON_PIN 15

// Core sleep time (cycle)
#define MILLIS_PER_CYCLE_MAIN_CORE (1000 / 4)
#define MILLIS_PER_CYCLE_UI_CORE (1000 / 30)

#define REQUEST_UPDATE 1

#endif //PICO_CLOCK_CONSTANTS_H

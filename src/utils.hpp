#ifndef PICO_CLOCK_UTILS_HPP
#define PICO_CLOCK_UTILS_HPP

#include "constants.hpp"
#include "pico/stdlib.h"
#include <string.h>

#if defined(DEBUG) && DEBUG > 0
#define log(fmt, args...) printf(fmt, ##args)
#else
#define log(fmt, args...) /* Don't do anything in release builds */
#endif

/**
 * Converts a byte (like uint8_t or int8_t) to a string with the binary representation of that byte
 * Used for debugging and logging
 * Taken from: https://stackoverflow.com/a/112956/7175336
 * @param x the byte to convert
 * @return a string with the binary representation of x
 */
const char *byte_to_binary(const uint8_t x);

/**
 * Reverses the bits in the number n, used to convert between least-significant-bit first and most-significant-bit first
 * Used before and after sending and receiving bytes from the DS1302 which uses LSB order which the pico sdk does not
 * support
 * Taken from: https://stackoverflow.com/a/34236981/7175336
 * @param n the byte to convert
 * @return the byte with the bits reversed (ie what was first is now last and what was last is now first)
 */
uint8_t reverse_bits(const uint8_t n);

#endif //PICO_CLOCK_UTILS_HPP

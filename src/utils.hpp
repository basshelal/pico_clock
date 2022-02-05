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

/**
 * Get the bits from a value from fromBit inclusive to toBit inclusive
 * If fromBit or toBit are not in the range 0-7 inclusive, or if fromBit is less than toBit, the original value is
 * returned, if fromBit == toBit then that means select that bit
 * @param value the value to select the bits from
 * @param fromBit the bit index to begin selection, inclusive ranging from 0 to 7
 * @param toBit the bit index to end selection, inclusive ranging from 0 to 7
 * @return the new value representing bits from the passed in value
 */
uint8_t get_bits(const uint8_t value, const uint8_t fromBit, const uint8_t toBit);

void set_bits(uint8_t *value, const uint8_t fromBit, const uint8_t toBit, const uint8_t bits);

#endif //PICO_CLOCK_UTILS_HPP

#ifndef PICO_CLOCK_UTILS_H
#define PICO_CLOCK_UTILS_H

#include "constants.h"
#include "pico/stdlib.h"
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DEBUG) && DEBUG > 0
#define log(fmt, args...) printf(fmt, ##args)
#else
#define log(fmt, args...) /* Don't do anything in release builds */
#endif

uint64_t getCyclesPassed();

void finishCycle();

/**
 * Clears the console
 * Taken from: https://stackoverflow.com/a/7660837/7175336
 */
inline void clear_console() {
    printf("\e[1;1H\e[2J");
}

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
inline uint8_t reverse_bits(const uint8_t n) {
    uint8_t r, i;
    for (r = 0, i = 0; i < 8; ++i)
        r |= ((n >> i) & 1) << (8 - i - 1);
    return r;
}

/**
 * Get the bits from a value from fromBit inclusive to toBit inclusive
 * If fromBit or toBit are not in the range 0-7 inclusive, or if fromBit is less than toBit, the original value is
 * returned, if fromBit == toBit then that means select that bit
 * @param value the value to select the bits from
 * @param fromBit the bit index to begin selection, inclusive ranging from 0 to 7
 * @param toBit the bit index to end selection, inclusive ranging from 0 to 7
 * @return the new value representing bits from the passed in value
 */
inline uint8_t get_bits(const uint8_t value, const uint8_t fromBit, const uint8_t toBit) {
    if (fromBit < 0 || fromBit > 7 || toBit < 0 || toBit > 7 || toBit < fromBit)
        return value;
    // If fromBit and toBit are out of range or invalid, just return the original value

    const uint8_t numberOfBits = (toBit - fromBit) + 1;
    // number of bits that the final result will have, which is the number of bits the user wanted
    // +1 because it's inclusive on both ends and we start from 0

    const uint8_t mask = 0xFF >> (8 - numberOfBits);
    // we need to get a mask to get only the `numberOfBits` from the original number
    // we get the mask by shifting right on 0xFF until we only have `numberOfBits` 1s at the end of the number
    // 0xFF     -> 1111_1111
    // >> 8 - 2 -> 0000_0011
    // that is the mask we will use with AND to remove 6 bits later

    const uint8_t shiftBy = 7 - toBit;
    // accommodate to the user's requested bit range, if they only wanted bits 2-3 then throw out the last 4 bits
    // value    -> 0111_0101
    // >> 7 - 3 -> 0000_0111
    // this removes the right side (0101 in value) but not the left side yet

    const uint8_t result = (value >> shiftBy) & mask;
    // remove the left side by ANDing with the mask we made which will have 1s in the last `numberOfBits`
    // value >> shiftBy                -> 0000_0111
    // & 0000_0011 (numberOfBits == 2) -> 0000_0011
    // we have removed both sides now
    return result;
}

inline uint8_t set_bits(const uint8_t value, const uint8_t fromBit, const uint8_t toBit, const uint8_t bits) {
    if (fromBit < 0 || fromBit > 7 || toBit < 0 || toBit > 7 || toBit < fromBit)
        return value;
    // If fromBit and toBit are out of range or invalid, just return the original value

    const uint8_t numberOfBits = (toBit - fromBit) + 1;
    // number of bits that the final result will have, which is the number of bits the user wanted
    // +1 because it's inclusive on both ends and we start from 0

    const uint8_t leftSideLength = fromBit;
    const uint8_t rightSideLength = 7 - toBit;

    // original -> 0101_1101
    // bits     -> xx10_10xx

    const uint8_t bitsMoved = bits << (7 - toBit);
    // move the bits value user provided into its correct location by shifting it to fromBit
    // bits (before move) -> 0000_1010
    // bits (after move)  -> 0010_1000

    const uint8_t shiftLeftSideBy = numberOfBits + rightSideLength;
    const uint8_t leftSide = ((uint8_t) (value >> shiftLeftSideBy)) << shiftLeftSideBy;
    // save left side of original by shifting it right then back to its original place
    // original   -> 0101_1101
    // >> fromBit -> 0000_0001
    // << fromBit -> 0100_0000

    const uint8_t shiftRightSideBy = numberOfBits + leftSideLength;
    const uint8_t rightSide = ((uint8_t) (value << shiftRightSideBy)) >> shiftRightSideBy;
    // save right side of original by shifting it left then back to its original place
    // original   -> 0101_1101
    // << toBit   -> 0100_0000
    // >> toBit   -> 0000_0001

    const uint8_t result = 0x00 | leftSide | rightSide | bitsMoved;
    // OR all 3 numbers, left side, right side, bits into a 0
    // 0            -> 0000_0000
    // | left side  -> 0100_0000
    // | right side -> 0100_0001
    // | bits       -> 0110_1001

    return result;
}

uint8_t get_font_index_for_char(const char c);

inline uint64_t secondsToCycles(float seconds) {
    const float cyclesPerSecond = (1000.0F / (float) MAIN_CORE_CYCLE);
    return (uint64_t) (cyclesPerSecond * seconds);
}

inline float cyclesToSeconds(uint64_t cycles) {
    const float cyclesPerSecond = (1000.0F / (float) MAIN_CORE_CYCLE);
    return (uint64_t) (((float) cycles) / cyclesPerSecond);
}

#ifdef __cplusplus
}
#endif

#endif //PICO_CLOCK_UTILS_H

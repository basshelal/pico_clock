#include "utils.hpp"

const char *byte_to_binary(const uint8_t x) {
    static char b[9];
    b[0] = '\0';
    int z;
    for (z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}

uint8_t reverse_bits(const uint8_t n) {
    uint8_t r, i;
    for (r = 0, i = 0; i < 8; ++i)
        r |= ((n >> i) & 1) << (8 - i - 1);
    return r;
}

uint8_t get_bits(const uint8_t value, const uint8_t fromBit, const uint8_t toBit) {
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

void set_bits(uint8_t *value, const uint8_t fromBit, const uint8_t toBit, const uint8_t bits) {
    // original -> 0101_1101
    // bits     -> xx10_10xx

    // save left side of original by shifting it right then back
    // original   -> 0101_1101
    // >> fromBit -> 0000_0001
    // << fromBit -> 0100_0000

    // save right side of original by shifting it left then back
    // original   -> 0101_1101
    // << toBit   -> 0100_0000
    // >> toBit   -> 0000_0001

    // OR all 3 numbers, left side, right side, bits into a 0
    // 0            -> 0000_0000
    // | left side  -> 0100_0000
    // | right side -> 0100_0001
    // | bits       -> 0110_1001
}
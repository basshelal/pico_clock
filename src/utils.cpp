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
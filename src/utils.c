#include "utils.h"

private uint64_t cyclesPassed = 0;

public uint64_t getCyclesPassed() { return cyclesPassed; }

public void finishCycle() { cyclesPassed++; }

public const char *byteToBinary(const uint8_t x) {
    static char b[9];
    b[0] = '\0';
    int z;
    for (z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}

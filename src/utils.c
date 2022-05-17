#include "utils.h"
#include "types.h"
#include "stdlib.h"
#include <string.h>

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

public void substring(const char *string, int fromIndex, int toIndex, char *result) {
    if (!string || !result ||
        fromIndex < 0 || toIndex < 0 ||
        fromIndex > toIndex)
        return;
    const int stringLength = strlen(string);
    const int requestedLength = toIndex - fromIndex;
    if (stringLength <= 0 || requestedLength <= 0) {
        strlcpy(result, NULL, strlen(result) + 1);
        return;
    }
    if (stringLength <= 0 || toIndex > stringLength)
        return;
    strlcpy(result, string + fromIndex, requestedLength + 1);
}

public Color getRandomColor() {
    Color result;
    result.r = (uint8_t) random();
    result.g = (uint8_t) random();
    result.b = (uint8_t) random();
    return result;
}
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

public void substring(const char *string, int fromIndex, int toIndex, char *result) {
    if (!string || !result ||
        fromIndex < 0 || toIndex < 0 ||
        fromIndex > toIndex)
        return;
    const int stringLength = strlen(string);
    const int resultLength = strlen(result);
    const int requestedLength = (toIndex - fromIndex) + 1;
    if (stringLength <= 0 || toIndex > stringLength)
        return;
    strncpy(result, string + fromIndex, requestedLength - 1);
}
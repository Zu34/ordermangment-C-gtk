#include <stdio.h>
#include "utils.h"

void log_message(const char *msg) {
    printf("[LOG]: %s\n", msg);
}

int clamp_int(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

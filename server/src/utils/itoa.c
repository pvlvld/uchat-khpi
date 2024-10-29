#include <stdbool.h>

char *itoa(int value, char *buffer) {
    int i = 0;
    bool is_negative = false;

    if (value < 0) {
        is_negative = true;
        value = -value;
    }

    do {
        buffer[i++] = "0123456789"[value % 10];
        value /= 10;
    } while (value > 0);

    if (is_negative) buffer[i++] = '-';

    buffer[i] = '\0';

    return buffer;
}
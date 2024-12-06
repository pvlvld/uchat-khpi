#include <stdio.h>

char *itoa(int value, char *buffer) {
    sprintf(buffer, "%d", value);
    return buffer;
}

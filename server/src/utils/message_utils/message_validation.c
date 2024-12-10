#include <string.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>

bool is_valid_message(char *message) {
    if (message == NULL) {
        printf("Message is NULL\n");
        return false;
    }

    // Check if the message length is within the allowed range
    size_t length = strlen(message);
    if (length < 1 || length > 5000) {
        printf("Invalid message length: %zu\n", length);
        return false;
    }

    // Define a regex pattern to allow letters, numbers, spaces, and specific special characters
    regex_t regex_message;
    const char *pattern = "^([a-zA-Z\\s\\d]|[\\?\\\\\\*\\+\\-\\|\\$\\[\\]\\{\\}\\<\\>\\.\\(\\)]|[!@#%&\"'`~_=/])+$";

    // Compile the regex
    if (regcomp(&regex_message, pattern, REG_EXTENDED) != 0) {
        printf("Regex compilation failed\n");
        return false;
    }

    // Debug the message character by character
    for (size_t i = 0; i < length; i++) {
        char test[2] = {message[i], '\0'};
        if (regexec(&regex_message, test, 0, NULL, 0) != 0) {
            printf("Invalid character found: '%c' at position %zu\n", message[i], i);
        }
    }

    // Check if the entire message matches the pattern
    int result = regexec(&regex_message, message, 0, NULL, 0);
    if (result != 0) {
        printf("Regex did not match the message: '%s'\n", message);
    } else {
        printf("Regex matched successfully!\n");
    }

    regfree(&regex_message);
    return (result == 0);
}


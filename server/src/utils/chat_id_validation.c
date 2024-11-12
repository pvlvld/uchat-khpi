#include <stdbool.h>
#include <ctype.h>

bool is_valid_chat_id(const char *chat_id_str) {
    if (chat_id_str == NULL || *chat_id_str == '\0') {
        return false;
    }

    for (int i = 0; chat_id_str[i] != '\0'; i++) {
        if (!isdigit(chat_id_str[i])) {
            return false; // If any character is not a digit, it's not valid
        }
    }

    return true;
}

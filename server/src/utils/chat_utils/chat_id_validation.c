#include <stdbool.h>
#include <ctype.h>
#include "../../../inc/utils.h"

bool is_valid_chat_id(const char *chat_id_str) {
    if (chat_id_str == NULL || *chat_id_str == '\0') return false;

    for (int i = 0; chat_id_str[i] != '\0'; i++) {
        // If any character is not a digit, it's not valid
        if (!isdigit(chat_id_str[i])) return false;
    }

    return true;
}

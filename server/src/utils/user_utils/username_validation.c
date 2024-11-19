#include <string.h>
#include <regex.h>
#include <stdbool.h>

bool is_valid_username(const char *username) {
    if (username == NULL) return false;
    // Check if the username is at least 3 characters long
    if (strlen(username) < 3 || strlen(username) > 100) return false;

    // Check if the username only contains letters, digits, and underscores
    regex_t regex_username;
    if (regcomp(&regex_username, "^[a-zA-Z0-9_]+$", REG_EXTENDED) != 0) return false;
    if (regexec(&regex_username, username, 0, NULL, 0) != 0) {
        regfree(&regex_username);
        return false;
    }
    regfree(&regex_username);

    return true;
}

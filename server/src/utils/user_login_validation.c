#include <regex.h>
#include <stdbool.h>
#include <string.h>

bool is_valid_login(const char *login) {
    // Check if the login is at least 5 characters long
    if (strlen(login) < 5 || strlen(login) > 50) return false;

    // Check if the login only contains letters and numbers
    regex_t regex_login;
    if (regcomp(&regex_login, "^[A-Za-z0-9]+$", REG_EXTENDED) != 0) return false;
    if (regexec(&regex_login, login, 0, NULL, 0) != 0) {
        regfree(&regex_login);
        return false;
    }
    regfree(&regex_login);

    return true;
}

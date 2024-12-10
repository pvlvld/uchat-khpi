#include <regex.h>
#include <stdbool.h>
#include <string.h>

bool is_valid_password(const char *password) {
    if (strlen(password) < 8 || strlen(password) > 100) return false;

    // Check if the password contains at least one letter
    regex_t regex_letter;
    if (regcomp(&regex_letter, ".*[A-Za-z].*", REG_EXTENDED) != 0) return false;
    if (regexec(&regex_letter, password, 0, NULL, 0) != 0) {
        regfree(&regex_letter);
        return false;
    }
    regfree(&regex_letter);

    // Check if the password contains at least one digit
    regex_t regex_digit;
    if (regcomp(&regex_digit, ".*[0-9].*", REG_EXTENDED) != 0) return false;
    if (regexec(&regex_digit, password, 0, NULL, 0) != 0) {
        regfree(&regex_digit);
        return false;
    }
    regfree(&regex_digit);

    // Check if the password contains at least one special character
    regex_t regex_special;
    // Define a regex pattern that matches at least one special character from the defined set
    if (regcomp(&regex_special, ".*[!@#$%^&*()-+=<>?].*", REG_EXTENDED) != 0) return false;

    if (regexec(&regex_special, password, 0, NULL, 0) != 0) {
        regfree(&regex_special);
        return false;
    }
    regfree(&regex_special);

    return true;
}

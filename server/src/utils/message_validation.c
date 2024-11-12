#include <string.h>
#include <regex.h>
#include <stdbool.h>

bool is_valid_message(const char *message) {
    if (strlen(message) < 1 || strlen(message) > 5000) return false;

    regex_t regex_letters, regex_numbers, regex_special_chars;

    if (regcomp(&regex_letters, "[A-Za-z]", REG_EXTENDED) != 0) return false;
    if (regcomp(&regex_numbers, "[0-9]", REG_EXTENDED) != 0) return false;
    if (regcomp(&regex_special_chars, "[ .,!?@#$%^&*()_+\\-=\\[\\]{};:'\"|\\\\<>?/`~]", REG_EXTENDED) != 0) return false;

    // Execute each regex to check for letters, numbers, and special characters
    bool has_letters = (regexec(&regex_letters, message, 0, NULL, 0) == 0);
    bool has_numbers = (regexec(&regex_numbers, message, 0, NULL, 0) == 0);
    bool only_allowed_chars = (regexec(&regex_special_chars, message, 0, NULL, 0) == 0);

    regfree(&regex_letters);
    regfree(&regex_numbers);
    regfree(&regex_special_chars);

    // The message is valid if it has letters or numbers and only allowed characters
    return (has_letters || has_numbers) && only_allowed_chars;
}

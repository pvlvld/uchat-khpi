#include <string.h>
#include <regex.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include "../../../../libraries/cJSON/cJSON.h"

bool is_valid_user_id(const char *user_id) {
    if (user_id == NULL) return false;
    // Check if the user_id is at least 1 character long
    if (strlen(user_id) < 1) return false;

    // Check if the user_id only contains numbers
    regex_t regex_user_id;
    if (regcomp(&regex_user_id, "^[0-9]+$", REG_EXTENDED) != 0) return false;
    if (regexec(&regex_user_id, user_id, 0, NULL, 0) != 0) {
        regfree(&regex_user_id);
        return false;
    }
    regfree(&regex_user_id);

    return (atoi(user_id) < INT_MAX);
}


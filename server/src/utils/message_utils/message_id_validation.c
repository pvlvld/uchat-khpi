#include <string.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

bool is_valid_message_id(char *message_id_str) {
        if (message_id_str == NULL) {
                return false;
        }

        regex_t regex;
        int ret;
        const char *pattern = "^[0-9]+$"; // Regex to match only digits

        // Compile the regex
        ret = regcomp(&regex, pattern, REG_EXTENDED);
        if (ret) {
            fprintf(stderr, "Could not compile regex\n");
            return false;
        }

        // Execute the regex
        ret = regexec(&regex, message_id_str, 0, NULL, 0);
        regfree(&regex); // Free the compiled regex

        // Return true if the regex matches, false otherwise
        if (!ret) {
            return (atoi(message_id_str) > 0 && atoi(message_id_str) < INT_MAX);
        } else if (ret == REG_NOMATCH) {
            return false;
        } else {
            fprintf(stderr, "Regex match failed\n");
            return false;
        }
        return true;
}

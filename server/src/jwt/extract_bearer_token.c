#include "../../inc/header.h"

char *extract_bearer_token(const char *request) {
    const char *bearer_prefix = "Authorization: Bearer ";
    const char *bearer_start = strstr(request, bearer_prefix);
    if (bearer_start) {
        bearer_start += strlen(bearer_prefix);
        const char *bearer_end = strchr(bearer_start, '\r');
        if (bearer_end) {
            size_t token_len = bearer_end - bearer_start;
            char *token = malloc(token_len + 1);
            if (!token) {
                return NULL; // Memory allocation error
            }
            strncpy(token, bearer_start, token_len);
            token[token_len] = '\0';
            return token;
        }
    }
    return NULL; // Token not found
}

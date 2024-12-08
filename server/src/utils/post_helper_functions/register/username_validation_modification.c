#include "../../../../inc/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *duplicate_string(const char *str) {
    size_t len = strlen(str) + 1;
    char *copy = malloc(len);
    if (copy) { memcpy(copy, str, len); }
    return copy;
}

// Function to find a free username by appending numbers until an available one is found
char *first_free_username(const char *username, PGconn *conn) {
    PGresult *res = get_user_by_username(conn, username);
    if (!res) { // Username is free
        return (char *)username;
    }
    int suffix = 1;
    size_t base_len = strlen(username);
    char *candidate = malloc(base_len + 12);

    if (!candidate) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    while (suffix < 1000) {
        snprintf(candidate, base_len + 12, "%s%d", username, suffix);

        // Check if this candidate username is free in the database
        PGresult *res = get_user_by_username(conn, candidate);
        if (!res) { // Username is free
            return candidate;
        }
        PQclear(res); // Username is occupied - try the next suffix
        suffix++;
    }

    free(candidate);
    return NULL; // If we exhaust suffix options, return NULL
}

char *username_validation(const char *username, const char *user_login, PGconn *conn) {
    // Check if the provided username is NULL or empty
    if (username == NULL || strlen(username) == 0) {
        // Generate a unique username based on the user login
        char *new_username = first_free_username(user_login, conn);
        return new_username ? new_username : NULL; // Return new username or NULL if no suggestions
    }

    if (!is_valid_username(username)) {
        return NULL; // Username is invalid
    }

    // Check if the username is already taken
    PGresult *res = get_user_by_username(conn, username);
    if (!res) {
        return (char *)username; // Username is available
    }

    // Username is occupied, so generate a new available username
    PQclear(res);
    char *new_username = first_free_username(username, conn);
    return new_username; // Return the new suggestion, or NULL if no free username was found
    // Caller should free this
}

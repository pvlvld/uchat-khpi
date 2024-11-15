#include "../../inc/header.h"

static char** split_string_by_newline(const char *str, int *count) {
    char *str_copy = vendor.helpers.strdup(str);
    if (!str_copy) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    *count = 0;
    char *token = strtok(str_copy, "\n");
    while (token != NULL) {
        (*count)++;
        token = strtok(NULL, "\n");
    }

    char **result = (char **)malloc((*count) * sizeof(char *));
    if (!result) {
        printf("Memory allocation failed\n");
        free(str_copy);
        return NULL;
    }

    strcpy(str_copy, str);
    int i = 0;
    token = strtok(str_copy, "\n");
    while (token != NULL) {
        result[i] = vendor.helpers.strdup(token);
        if (!result[i]) {
            printf("Memory allocation failed\n");
            for (int j = 0; j < i; j++) {
                free(result[j]);
            }
            free(result);
            free(str_copy);
            return NULL;
        }
        i++;
        token = strtok(NULL, "\n");
    }

    free(str_copy);
    return result;
}

static void free_split_result(char **result, int count) {
    for (int i = 0; i < count; i++) {
        free(result[i]);
    }
    free(result);
}

char *encrypt_data_for_db(char *public_key_str, const char *text) {
    char *key = vendor.crypto.generate_random_key();
    char *encrypted_key = vendor.crypto.encrypt(public_key_str, key);
    char *encrypted_text = vendor.crypto.encrypt_text(text, key);

    char *result = malloc((strlen(encrypted_key) + strlen(encrypted_text) + 1) * sizeof(char *));
    sprintf(result, "%s\n%s", encrypted_key, encrypted_text);

    free(encrypted_key);
    free(encrypted_text);
    free(key);

    return result;
}

char *decrypt_data_from_db(const char *text) {
    int count = 0;
    char **result = split_string_by_newline(text, &count);
    if (result) {
        char *decrypted_key = vendor.crypto.decrypt(result[0]);
        char *decrypted_text = vendor.crypto.decrypt_text(result[1], decrypted_key);

        free_split_result(result, count);

        return decrypted_text;
    }

    return NULL;
}

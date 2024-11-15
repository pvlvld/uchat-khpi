#include "../../inc/header.h"

char *generate_random_key(void) {
    char *key;
    unsigned char raw_key[AES_KEY_LENGTH];

    if (!RAND_bytes(raw_key, AES_KEY_LENGTH)) {
        fprintf(stderr, "Ошибка генерации случайного ключа\n");
        exit(EXIT_FAILURE);
    }

    key = malloc(AES_KEY_LENGTH * 2 + 1);
    if (key == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для ключа\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < AES_KEY_LENGTH; i++) {
        sprintf(key + i * 2, "%02x", raw_key[i]);
    }

    key[AES_KEY_LENGTH * 2] = '\0';
    return key;
}

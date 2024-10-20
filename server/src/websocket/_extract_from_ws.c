#include "../../inc/websocket.h"

char *_extract_from_ws(const char *key, const char *request) {
    const char *extract_key = key;
    char *key_start = strstr(request, extract_key);
    if (key_start) {
        key_start += strlen(extract_key);
        char *key_end = strchr(key_start, '&');
        if (!key_end) {
            key_end = strchr(key_start, ' ');
        }
        if (key_end) {
            size_t extract_len = key_end - key_start;
            char *extract_str = malloc(extract_len + 1);
            if (!extract_str) {
                return NULL; // Ошибка выделения памяти
            }
            strncpy(extract_str, key_start, extract_len);
            extract_str[extract_len] = '\0';
            return extract_str;
        }
    }
    return NULL; // Ключ не найден
}

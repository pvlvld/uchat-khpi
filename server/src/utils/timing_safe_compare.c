int timing_safe_compare(const void *a, const void *b, unsigned long len) {
    const unsigned char *ua = (const unsigned char *)a;
    const unsigned char *ub = (const unsigned char *)b;
    unsigned char result = 0;

    for (unsigned long i = 0; i < len; i++) result |= ua[i] ^ ub[i];

    return result != 0;
}

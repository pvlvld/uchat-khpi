#include "../../inc/header.h"

t_crypto init_crypto(void) {
    t_crypto crypto = {
        .public_key_str = NULL,
        .private_key_str = NULL,
        .keygen = keygen,
        .encrypt = encrypt,
        .decrypt = decrypt,
        .encrypt_text = encrypt_text,
        .decrypt_text = decrypt_text,
        .verify_key_pair = verify_key_pair,
    };

    return crypto;
}

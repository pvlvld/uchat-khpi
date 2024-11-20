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
        .generate_random_key = generate_random_key,
        .verify_key_pair = verify_key_pair,
        .encrypt_data_for_db = encrypt_data_for_db,
        .decrypt_data_from_db = decrypt_data_from_db,
    };

    return crypto;
}

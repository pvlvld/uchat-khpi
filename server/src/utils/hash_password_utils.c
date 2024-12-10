#include "../../../libraries/openssl/evp.h"
#include "../../../libraries/openssl/kdf.h"
#include "../../../libraries/openssl/rand.h"
#include "../../inc/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Hash a password using Argon2i KDF
 *
 * @param password Input password to hash
 * @param salt Cryptographic salt (should be randomly generated)
 * @return Dynamically allocated buffer with the hash, or NULL on error
 * @note Caller is responsible for freeing the returned buffer
 */

// temp
char *hash_password(const char *password, const char *salt) {
    (void) salt;
    EVP_MD_CTX *mdctx;
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length;

    // Create and initialize the context
    mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        printf("Failed to create EVP_MD_CTX\n");
        return NULL;
    }

    // Initialize the digest context for SHA-256
    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
        printf("EVP_DigestInit_ex failed\n");
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Update the digest with the password
    if (EVP_DigestUpdate(mdctx, password, strlen(password)) != 1) {
        printf("EVP_DigestUpdate failed\n");
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Finalize the digest
    if (EVP_DigestFinal_ex(mdctx, hash, &hash_length) != 1) {
        printf("EVP_DigestFinal_ex failed\n");
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Allocate memory for the output hash string (hex representation)
    char *hashed_output = (char *)malloc((hash_length * 2) + 1);
    if (hashed_output == NULL) {
        printf("Failed to allocate memory for hashed output\n");
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Convert the hash to a hexadecimal string
    for (unsigned int i = 0; i < hash_length; i++) {
        sprintf(hashed_output + (i * 2), "%02x", hash[i]);
    }
    hashed_output[hash_length * 2] = '\0';

    // Clean up
    EVP_MD_CTX_free(mdctx);

    return hashed_output;
}

//char *hash_password(const char *password, const char *salt) {
//    if (!password || !salt) {
//        fprintf(stderr, "Invalid input parameters\n");
//        return NULL;
//    }
//
//    // Combine password with pepper
//    size_t password_len = strlen(password);
//    size_t pepper_len = strlen(ARGON_PEPPER);
//    size_t combined_len = password_len + pepper_len;
//
//    char *combined_password = malloc(combined_len + 1);
//    if (!combined_password) {
//        fprintf(stderr, "Memory allocation failed for combined password\n");
//        return NULL;
//    }
//
//    // Concatenate password and pepper
//    memcpy(combined_password, password, password_len);
//    memcpy(combined_password + password_len, ARGON_PEPPER, pepper_len);
//    combined_password[combined_len] = '\0';
//
//    // Allocate memory for hash
//    unsigned char *hash = malloc(ARGON_HASH_LENGTH);
//    if (!hash) {
//        fprintf(stderr, "Memory allocation failed for hash\n");
//        free(combined_password);
//        return NULL;
//    }
//
//    // Fetch Argon2i KDF
//    EVP_KDF *kdf = EVP_KDF_fetch(NULL, "argon2i", NULL);
//    if (!kdf) {
//        fprintf(stderr, "Failed to fetch Argon2i KDF\n");
//        free(hash);
//        free(combined_password);
//        return NULL;
//    }
//
//    // Create KDF context
//    EVP_KDF_CTX *kctx = EVP_KDF_CTX_new(kdf);
//    if (!kctx) {
//        fprintf(stderr, "Failed to create KDF context\n");
//        EVP_KDF_free(kdf);
//        free(hash);
//        free(combined_password);
//        return NULL;
//    }
//
//    // Prepare Argon2 parameters
//    OSSL_PARAM params[] = {OSSL_PARAM_construct_utf8_string("pass", combined_password, combined_len),
//                           OSSL_PARAM_construct_utf8_string("salt", (void *)salt, strlen(salt)),
//                           OSSL_PARAM_construct_int("iterations", &(int){ARGON_ITERATIONS}),
//                           OSSL_PARAM_construct_int("memory", &(int){ARGON_MEMORY}),
//                           OSSL_PARAM_construct_int("threads", &(int){ARGON_THREADS}),
//                           OSSL_PARAM_construct_end()};
//
//    // Derive hash
//    int derive_result = EVP_KDF_derive(kctx, hash, ARGON_HASH_LENGTH, params);
//
//    // Cleanup
//    EVP_KDF_CTX_free(kctx);
//    EVP_KDF_free(kdf);
//
//    // Securely clear the combined password from memory
//    memset(combined_password, 0, combined_len);
//    free(combined_password);
//
//    // Check derive result
//    if (derive_result != 1) {
//        fprintf(stderr, "Error deriving Argon2 hash\n");
//        free(hash);
//        return NULL;
//    }
//
//    return (char *)hash;
//}

char *generate_salt(void) {
    unsigned char *salt = malloc(ARGON_SALT_LENGTH + 1);
    if (!salt) {
        fprintf(stderr, "Memory allocation failed for salt\n");
        return NULL;
    }

    if (RAND_bytes(salt, ARGON_SALT_LENGTH) != 1) {
        fprintf(stderr, "Failed to generate cryptographically secure random salt\n");
        free(salt);
        return NULL;
    }

    salt[ARGON_SALT_LENGTH] = '\0';

    return (char *)salt;
}

// Example usage
// int main() {
//     const char *password = "my_secure_password";
//     const char *salt = "random_salt_value";

//     unsigned char *hashed_password = hash_password(password, salt);

//     if (hashed_password) {
//         // Print or use the hashed password
//         free(hashed_password);
//     }

//     return 0;
// }

#ifndef HASH_H
#define HASH_H

/** Hashes the given data using SHA-256.
 *
 * DO NOT USE THIS FUNCTION FOR PASSWORD HASHING!
 * @param data The data to hash.
 * @param outputBuffer The buffer to store the hash in.
 */
void sha256(unsigned char *data, unsigned char *outputBuffer);

#endif //HASH_H

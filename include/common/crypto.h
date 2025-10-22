#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/evp.h>
#include <stdlib.h>
#include <stdio.h>

// Key generation
EVP_PKEY* generate_key_pair();

// Encryption/Decryption
int encrypt_message(const unsigned char* plaintext, size_t plaintext_len, 
                   unsigned char* ciphertext, size_t* ciphertext_len, EVP_PKEY* public_key);
int decrypt_message(const unsigned char* ciphertext, size_t ciphertext_len,
                   unsigned char* plaintext, size_t* plaintext_len, EVP_PKEY* private_key);

// Base64 encoding
int base64_encode(const unsigned char* data, size_t data_len, char* output, size_t output_len);
int base64_decode(const char* data, unsigned char* output, size_t output_len, size_t* decoded_len);

// Client crypto functions
int init_crypto();
void cleanup_crypto();
EVP_PKEY* get_client_keypair();
int encrypt_message_for_server(const unsigned char* plaintext, size_t plaintext_len,
                              unsigned char* ciphertext, size_t* ciphertext_len);
int decrypt_message_from_server(const unsigned char* ciphertext, size_t ciphertext_len,
                               unsigned char* plaintext, size_t* plaintext_len);

#endif

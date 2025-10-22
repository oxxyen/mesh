#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include "../include/common/crypto.h"

EVP_PKEY* generate_key_pair() {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        printf("❌ Failed to create EVP_PKEY_CTX\n");
        return NULL;
    }
    
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        printf("❌ Failed to initialize key generation\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }
    
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        printf("❌ Failed to set RSA key bits\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }
    
    EVP_PKEY *pkey = NULL;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        printf("❌ Failed to generate key pair\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }
    
    EVP_PKEY_CTX_free(ctx);
    printf("✅ Generated RSA-2048 key pair\n");
    return pkey;
}

int encrypt_message(const unsigned char* plaintext, size_t plaintext_len, 
                   unsigned char* ciphertext, size_t* ciphertext_len, EVP_PKEY* public_key) {
    if (!plaintext || !ciphertext || !public_key) {
        printf("❌ Invalid parameters for encryption\n");
        return -1;
    }
    
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(public_key, NULL);
    if (!ctx) {
        printf("❌ Failed to create encryption context\n");
        return -1;
    }
    
    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        printf("❌ Failed to initialize encryption\n");
        EVP_PKEY_CTX_free(ctx);
        return -2;
    }
    
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        printf("❌ Failed to set RSA padding\n");
        EVP_PKEY_CTX_free(ctx);
        return -3;
    }
    
    if (EVP_PKEY_encrypt(ctx, ciphertext, ciphertext_len, plaintext, plaintext_len) <= 0) {
        printf("❌ Encryption failed\n");
        EVP_PKEY_CTX_free(ctx);
        return -4;
    }
    
    EVP_PKEY_CTX_free(ctx);
    printf("✅ Message encrypted successfully (%zd -> %zd bytes)\n", plaintext_len, *ciphertext_len);
    return 0;
}

int decrypt_message(const unsigned char* ciphertext, size_t ciphertext_len,
                   unsigned char* plaintext, size_t* plaintext_len, EVP_PKEY* private_key) {
    if (!ciphertext || !plaintext || !private_key) {
        printf("❌ Invalid parameters for decryption\n");
        return -1;
    }
    
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(private_key, NULL);
    if (!ctx) {
        printf("❌ Failed to create decryption context\n");
        return -1;
    }
    
    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        printf("❌ Failed to initialize decryption\n");
        EVP_PKEY_CTX_free(ctx);
        return -2;
    }
    
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        printf("❌ Failed to set RSA padding\n");
        EVP_PKEY_CTX_free(ctx);
        return -3;
    }
    
    if (EVP_PKEY_decrypt(ctx, plaintext, plaintext_len, ciphertext, ciphertext_len) <= 0) {
        printf("❌ Decryption failed\n");
        EVP_PKEY_CTX_free(ctx);
        return -4;
    }
    
    EVP_PKEY_CTX_free(ctx);
    printf("✅ Message decrypted successfully (%zd -> %zd bytes)\n", ciphertext_len, *plaintext_len);
    return 0;
}

int base64_encode(const unsigned char* data, size_t data_len, char* output, size_t output_len) {
    if (data == NULL || output == NULL) {
        printf("❌ Invalid parameters for base64 encode\n");
        return -1;
    }
    
    // Simplified base64 encoding for demo
    // In real implementation, use proper base64 library
    if (data_len * 2 >= output_len) {
        printf("❌ Output buffer too small for base64 encoding\n");
        return -2;
    }
    
    // Simple hex encoding for demo (not real base64)
    for (size_t i = 0; i < data_len; i++) {
        sprintf(output + i * 2, "%02x", data[i]);
    }
    output[data_len * 2] = '\0';
    
    printf("✅ Base64 encoded %zd bytes to %zd characters\n", data_len, strlen(output));
    return 0;
}

int base64_decode(const char* data, unsigned char* output, size_t output_len, size_t* decoded_len) {
    if (data == NULL || output == NULL) {
        printf("❌ Invalid parameters for base64 decode\n");
        return -1;
    }
    
    size_t input_len = strlen(data);
    if (input_len % 2 != 0 || input_len / 2 > output_len) {
        printf("❌ Invalid input length or output buffer too small\n");
        return -2;
    }
    
    // Simple hex decoding for demo (not real base64)
    for (size_t i = 0; i < input_len; i += 2) {
        sscanf(data + i, "%2hhx", &output[i / 2]);
    }
    *decoded_len = input_len / 2;
    
    printf("✅ Base64 decoded %zd characters to %zd bytes\n", input_len, *decoded_len);
    return 0;
}

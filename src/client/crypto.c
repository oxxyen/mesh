#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include "../include/common/crypto.h"

static EVP_PKEY* client_keypair = NULL;

int init_crypto() {
    printf("🔑 Initializing cryptography subsystem...\n");
    
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    
    // Generate client key pair
    client_keypair = generate_key_pair();
    if (client_keypair == NULL) {
        printf("❌ Failed to generate client key pair\n");
        return -1;
    }
    
    printf("✅ Cryptography initialized successfully\n");
    return 0;
}

void cleanup_crypto() {
    printf("🧹 Cleaning up cryptography resources...\n");
    
    if (client_keypair) {
        EVP_PKEY_free(client_keypair);
        client_keypair = NULL;
        printf("✅ Client key pair freed\n");
    }
    
    EVP_cleanup();
    ERR_free_strings();
    printf("✅ Cryptography cleanup complete\n");
}

EVP_PKEY* get_client_keypair() {
    return client_keypair;
}

int encrypt_message_for_server(const unsigned char* plaintext, size_t plaintext_len,
                              unsigned char* ciphertext, size_t* ciphertext_len) {
    printf("🔒 Encrypting message for server (%zd bytes)...\n", plaintext_len);
    
    // In real implementation, use server's public key
    // For now, simulate encryption by copying the data
    if (plaintext_len > *ciphertext_len) {
        printf("❌ Ciphertext buffer too small\n");
        return -1;
    }
    
    memcpy(ciphertext, plaintext, plaintext_len);
    *ciphertext_len = plaintext_len;
    
    printf("✅ Message encrypted (simulated)\n");
    return 0;
}

int decrypt_message_from_server(const unsigned char* ciphertext, size_t ciphertext_len,
                               unsigned char* plaintext, size_t* plaintext_len) {
    printf("🔓 Decrypting message from server (%zd bytes)...\n", ciphertext_len);
    
    // In real implementation, use client's private key
    // For now, simulate decryption by copying the data
    if (ciphertext_len > *plaintext_len) {
        printf("❌ Plaintext buffer too small\n");
        return -1;
    }
    
    memcpy(plaintext, ciphertext, ciphertext_len);
    *plaintext_len = ciphertext_len;
    
    printf("✅ Message decrypted (simulated)\n");
    return 0;
}

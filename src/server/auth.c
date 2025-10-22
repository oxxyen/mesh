#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../include/server/auth.h"
#include "../include/server/redis.h"
#include "../include/common/crypto.h"

int register_user(const char* username, const char* ip_address, unsigned char* public_key, size_t key_len) {
    if (username == NULL || public_key == NULL) {
        printf("❌ Invalid parameters for user registration\n");
        return -1;
    }
    
    printf("👤 Attempting to register user: %s from %s\n", username, ip_address);
    
    // Check if user already exists
    if (redis_user_exists(username) == 1) {
        printf("❌ User %s already exists\n", username);
        return -2; // User exists
    }
    
    // Generate user ID
    char user_id[32];
    snprintf(user_id, sizeof(user_id), "user_%ld", time(NULL));
    
    // Store user in Redis
    if (redis_store_user(user_id, username, ip_address, public_key, key_len) != 0) {
        printf("❌ Failed to store user %s in Redis\n", username);
        return -3; // Storage failed
    }
    
    printf("✅ Registered user: %s (%s)\n", username, user_id);
    return 0;
}

int authenticate_user(const char* username, const char* challenge, const char* response) {
    if (username == NULL || challenge == NULL || response == NULL) {
        printf("❌ Invalid parameters for authentication\n");
        return -1;
    }
    
    printf("🔑 Attempting to authenticate user: %s\n", username);
    
    // In real implementation, verify cryptographic challenge-response
    // For now, simple validation
    User user;
    if (redis_get_user(username, &user) != 0) {
        printf("❌ User %s not found\n", username);
        return -2; // User not found
    }
    
    // Verify challenge-response (simplified - in real app use crypto)
    if (strlen(response) > 0 && strlen(challenge) > 0) {
        printf("✅ User authenticated: %s\n", username);
        return 0;
    }
    
    printf("❌ Authentication failed for user: %s\n", username);
    return -3; // Authentication failed
}

int generate_auth_challenge(const char* username, char* challenge, size_t challenge_len) {
    if (challenge == NULL || challenge_len < 32) {
        printf("❌ Invalid challenge buffer\n");
        return -1;
    }
    
    // Generate random challenge
    srand(time(NULL));
    for (size_t i = 0; i < challenge_len - 1; i++) {
        challenge[i] = 'A' + (rand() % 26);
    }
    challenge[challenge_len - 1] = '\0';
    
    printf("🔐 Generated challenge for %s: %s\n", username, challenge);
    
    // Store challenge for verification
    redis_store_challenge(username, challenge);
    
    return 0;
}

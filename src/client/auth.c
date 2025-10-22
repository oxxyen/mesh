#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/client/auth.h"
#include "../include/client/network.h"
#include "../include/common/protocol.h"
#include "../include/common/crypto.h"

int authenticate_with_server(int sock) {
    printf("\n🔑 Authentication\n");
    printf("================\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("Choose option (1 or 2): ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("❌ Invalid input\n");
        return 0;
    }
    getchar(); // Consume newline
    
    if (choice != 1 && choice != 2) {
        printf("❌ Invalid choice. Please select 1 or 2.\n");
        return 0;
    }
    
    char username[64];
    printf("Username: ");
    if (fgets(username, sizeof(username), stdin) == NULL) {
        printf("❌ Failed to read username\n");
        return 0;
    }
    username[strcspn(username, "\n")] = '\0';
    
    if (strlen(username) == 0) {
        printf("❌ Username cannot be empty\n");
        return 0;
    }
    
    if (choice == 1) {
        // Registration
        return register_user(sock, username);
    } else {
        // Login
        return login_user(sock, username);
    }
}

int register_user(int sock, const char* username) {
    printf("📝 Registering user: %s\n", username);
    
    Message msg;
    msg.type = MSG_TYPE_REGISTER;
    msg.timestamp = time(NULL);
    
    // Generate simulated public key
    unsigned char public_key[256] = "SIMULATED_RSA_PUBLIC_KEY_";
    strncat((char*)public_key, username, 32);
    
    // Simulate base64 encoding
    char public_key_b64[512];
    snprintf(public_key_b64, sizeof(public_key_b64), "base64_%s_key", username);
    
    // Format: username:public_key
    snprintf(msg.data, sizeof(msg.data), "%s:%s", username, public_key_b64);
    
    printf("🔑 Sending registration request...\n");
    
    // Send registration request
    if (send_message(sock, &msg) != 0) {
        printf("❌ Failed to send registration request\n");
        return 0;
    }
    
    printf("⏳ Waiting for server response...\n");
    
    // Wait for response
    Message response;
    if (receive_message(sock, &response) != 0 || response.type != MSG_TYPE_REGISTER_RESPONSE) {
        printf("❌ Invalid response from server\n");
        return 0;
    }
    
    printf("📨 Server response: %s\n", response.data);
    
    if (strstr(response.data, "SUCCESS") != NULL) {
        printf("✅ Registration successful!\n");
        return 1;
    } else {
        printf("❌ Registration failed: %s\n", response.data);
        return 0;
    }
}

int login_user(int sock, const char* username) {
    printf("🔐 Logging in as: %s\n", username);
    
    Message msg;
    msg.type = MSG_TYPE_LOGIN;
    msg.timestamp = time(NULL);
    strncpy(msg.data, username, sizeof(msg.data) - 1);
    
    printf("🔑 Sending login request...\n");
    
    if (send_message(sock, &msg) != 0) {
        printf("❌ Failed to send login request\n");
        return 0;
    }
    
    printf("⏳ Waiting for server response...\n");
    
    Message response;
    if (receive_message(sock, &response) != 0 || response.type != MSG_TYPE_LOGIN_RESPONSE) {
        printf("❌ Invalid response from server\n");
        return 0;
    }
    
    printf("📨 Server response: %s\n", response.data);
    
    if (strstr(response.data, "SUCCESS") != NULL) {
        printf("✅ Login successful!\n");
        return 1;
    } else {
        printf("❌ Login failed: %s\n", response.data);
        return 0;
    }
}

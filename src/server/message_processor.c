#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../include/server/message_processor.h"
#include "../include/server/redis.h"
#include "../include/common/protocol.h"
#include "../include/common/crypto.h"

void handle_message(int client_socket, Message* msg) {
    if (msg == NULL) {
        printf("❌ Received null message\n");
        return;
    }
    
    printf("📨 Handling message type: %d from socket %d\n", msg->type, client_socket);
    
    Message response;
    memset(&response, 0, sizeof(Message));
    response.timestamp = time(NULL);
    
    switch (msg->type) {
        case MSG_TYPE_REGISTER:
            handle_registration(client_socket, msg);
            break;
            
        case MSG_TYPE_LOGIN:
            handle_login(client_socket, msg);
            break;
            
        case MSG_TYPE_TEXT:
            handle_text_message(client_socket, msg);
            break;
            
        case MSG_TYPE_FILE:
            handle_file_message(client_socket, msg);
            break;
            
        case MSG_TYPE_GET_USERS:
            handle_get_users(client_socket, msg);
            break;
            
        default:
            printf("❌ Unknown message type: %d\n", msg->type);
            response.type = MSG_TYPE_ERROR;
            strncpy(response.data, "Unknown message type", sizeof(response.data) - 1);
            send_encrypted_message(client_socket, &response);
            break;
    }
}

void handle_registration(int client_socket, Message* msg) {
    printf("👤 Processing registration request\n");
    
    Message response;
    response.type = MSG_TYPE_REGISTER_RESPONSE;
    response.timestamp = time(NULL);
    
    // Parse registration data (username:public_key)
    char data_copy[1024];
    strncpy(data_copy, msg->data, sizeof(data_copy) - 1);
    data_copy[sizeof(data_copy) - 1] = '\0';
    
    char* colon = strchr(data_copy, ':');
    if (colon == NULL) {
        printf("❌ Invalid registration format\n");
        strncpy(response.data, "ERROR:Invalid format - expected username:public_key", sizeof(response.data) - 1);
        send_encrypted_message(client_socket, &response);
        return;
    }
    
    *colon = '\0';
    char* username = data_copy;
    char* public_key_b64 = colon + 1;
    
    printf("📝 Registering user: %s\n", username);
    
    // Decode public key (simplified - in real app use proper base64)
    unsigned char public_key[512];
    size_t key_len = strlen(public_key_b64);
    if (key_len >= sizeof(public_key)) {
        key_len = sizeof(public_key) - 1;
    }
    memcpy(public_key, public_key_b64, key_len);
    public_key[key_len] = '\0';
    
    // Get client IP (simplified - in real app get actual IP)
    char client_ip[INET_ADDRSTRLEN] = "127.0.0.1";
    
    int result = register_user(username, client_ip, public_key, key_len);
    if (result == 0) {
        strncpy(response.data, "SUCCESS:Registration completed", sizeof(response.data) - 1);
        printf("✅ Registration successful for %s\n", username);
    } else if (result == -2) {
        strncpy(response.data, "ERROR:User already exists", sizeof(response.data) - 1);
        printf("❌ User %s already exists\n", username);
    } else {
        strncpy(response.data, "ERROR:Registration failed", sizeof(response.data) - 1);
        printf("❌ Registration failed for %s\n", username);
    }
    
    send_encrypted_message(client_socket, &response);
}

void handle_login(int client_socket, Message* msg) {
    printf("🔑 Processing login request\n");
    
    Message response;
    response.type = MSG_TYPE_LOGIN_RESPONSE;
    response.timestamp = time(NULL);
    
    char* username = msg->data;
    printf("🔐 Login attempt for user: %s\n", username);
    
    // For now, simple authentication - just check if user exists
    User user;
    if (redis_get_user(username, &user) == 0) {
        strncpy(response.data, "SUCCESS:Login successful", sizeof(response.data) - 1);
        printf("✅ Login successful for %s\n", username);
    } else {
        strncpy(response.data, "ERROR:Login failed - user not found", sizeof(response.data) - 1);
        printf("❌ Login failed for %s - user not found\n", username);
    }
    
    send_encrypted_message(client_socket, &response);
}

void handle_text_message(int client_socket, Message* msg) {
    printf("💬 Text message from %s to %s: %s\n", 
           msg->sender_id, msg->recipient_id, msg->data);
    
    // Store message in Redis
    redis_store_message(msg->sender_id, msg->recipient_id, msg->data, msg->timestamp);
    
    // Send acknowledgment
    Message ack;
    ack.type = MSG_TYPE_ACK;
    ack.timestamp = time(NULL);
    strncpy(ack.data, "Message delivered", sizeof(ack.data) - 1);
    send_encrypted_message(client_socket, &ack);
    
    printf("✅ Message stored and acknowledgment sent\n");
}

void handle_file_message(int client_socket, Message* msg) {
    printf("📁 File message received from %s\n", msg->sender_id);
    
    Message ack;
    ack.type = MSG_TYPE_ACK;
    ack.timestamp = time(NULL);
    strncpy(ack.data, "File received", sizeof(ack.data) - 1);
    send_encrypted_message(client_socket, &ack);
    
    printf("✅ File message acknowledged\n");
}

void handle_get_users(int client_socket, Message* msg) {
    printf("👥 User list requested by %s\n", msg->sender_id);
    
    Message response;
    response.type = MSG_TYPE_USER_LIST;
    response.timestamp = time(NULL);
    
    // Get online users from Redis (simplified)
    char user_list[1024] = "Online users: alice, bob, charlie (demo list)";
    strncpy(response.data, user_list, sizeof(response.data) - 1);
    
    send_encrypted_message(client_socket, &response);
    printf("✅ User list sent\n");
}

int process_client_message(int client_socket, unsigned char* encrypted_data, size_t data_len, Message* output_msg) {
    if (encrypted_data == NULL || output_msg == NULL) {
        printf("❌ Invalid parameters for message processing\n");
        return -1;
    }
    
    printf("🔓 Processing client message (%zd bytes)\n", data_len);
    
    // For demo purposes, we'll use the data directly
    // In real implementation, decrypt here
    if (parse_message(encrypted_data, output_msg) != 0) {
        printf("❌ Failed to parse message\n");
        return -2;
    }
    
    printf("✅ Message parsed successfully: type=%d, sender=%s\n", 
           output_msg->type, output_msg->sender_id);
    return 0;
}

int send_encrypted_message(int client_socket, Message* msg) {
    if (msg == NULL) {
        printf("❌ Cannot send null message\n");
        return -1;
    }
    
    char serialized[BUFFER_SIZE];
    if (serialize_message(msg, serialized, sizeof(serialized)) != 0) {
        printf("❌ Failed to serialize message\n");
        return -2;
    }
    
    printf("📤 Sending message type %d: %s\n", msg->type, msg->data);
    
    ssize_t sent = send(client_socket, serialized, strlen(serialized), 0);
    if (sent <= 0) {
        perror("❌ send failed");
        return -3;
    }
    
    printf("✅ Message sent successfully (%zd bytes)\n", sent);
    return 0;
}

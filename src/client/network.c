#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include "../include/client/network.h"
#include "../include/common/protocol.h"

int connect_to_server(const char* ip, int port) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    printf("🔌 Creating socket...\n");
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("❌ socket creation error");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    printf("🔗 Converting IP address...\n");
    
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        perror("❌ invalid address");
        close(sock);
        return -1;
    }
    
    printf("🚀 Connecting to %s:%d...\n", ip, port);
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("❌ connection failed");
        close(sock);
        return -1;
    }
    
    printf("✅ Connected to server successfully\n");
    return sock;
}

int send_message(int sock, Message* msg) {
    if (sock < 0 || msg == NULL) {
        printf("❌ Invalid socket or message\n");
        return -1;
    }
    
    char buffer[BUFFER_SIZE];
    if (serialize_message(msg, buffer, sizeof(buffer)) != 0) {
        printf("❌ Failed to serialize message\n");
        return -2;
    }
    
    printf("📤 Sending message type %d...\n", msg->type);
    
    ssize_t bytes_sent = send(sock, buffer, strlen(buffer), 0);
    if (bytes_sent <= 0) {
        perror("❌ send failed");
        return -3;
    }
    
    printf("✅ Message sent successfully (%zd bytes)\n", bytes_sent);
    return 0;
}

int receive_message(int sock, Message* msg) {
    if (sock < 0 || msg == NULL) {
        printf("❌ Invalid socket or message buffer\n");
        return -1;
    }
    
    char buffer[BUFFER_SIZE];
    
    printf("📥 Waiting for server response...\n");
    
    ssize_t bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            printf("🔌 Server closed the connection\n");
        } else {
            perror("❌ recv failed");
        }
        return -2;
    }
    
    buffer[bytes_received] = '\0';
    printf("📨 Received %zd bytes from server\n", bytes_received);
    
    int result = parse_message((unsigned char*)buffer, msg);
    if (result != 0) {
        printf("❌ Failed to parse message: error %d\n", result);
        return -3;
    }
    
    printf("✅ Message parsed: type=%d\n", msg->type);
    return 0;
}

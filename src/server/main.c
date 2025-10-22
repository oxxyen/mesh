#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include "../include/server/auth.h"
#include "../include/server/message_processor.h"
#include "../include/server/redis.h"
#include "../include/common/protocol.h"

#define PORT 8888
#define MAX_CLIENTS 100

static int server_running = 1;

void handle_signal(int sig) {
    server_running = 0;
    printf("\n🛑 Received signal %d, shutting down...\n", sig);
}

void* client_handler(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    
    unsigned char buffer[BUFFER_SIZE];
    Message msg;
    
    printf("🎯 Started client handler for socket %d\n", client_socket);
    
    while (server_running) {
        ssize_t bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read <= 0) {
            printf("🔌 Client disconnected or error on socket %d\n", client_socket);
            break;
        }
        
        buffer[bytes_read] = '\0';
        printf("📨 Received %zd bytes from client %d\n", bytes_read, client_socket);
        
        // Process message
        if (process_client_message(client_socket, buffer, bytes_read, &msg) == 0) {
            handle_message(client_socket, &msg);
        } else {
            printf("❌ Failed to process message from client %d\n", client_socket);
        }
    }
    
    close(client_socket);
    printf("🔚 Client handler finished for socket %d\n", client_socket);
    return NULL;
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t thread_id;
    
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    printf("🚀 Starting Secure Messenger Server...\n");
    
    // Initialize Redis connection
    printf("🔗 Connecting to Redis...\n");
    if (init_redis() != 0) {
        fprintf(stderr, "❌ Failed to initialize Redis\n");
        return 1;
    }
    
    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("❌ socket failed");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("❌ setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("❌ bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("❌ listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("✅ Server listening on port %d\n", PORT);
    printf("📍 Server IP: 127.0.0.1:%d\n", PORT);
    printf("⏹️  Press Ctrl+C to stop the server\n\n");
    
    while (server_running) {
        printf("👂 Waiting for connections...\n");
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            if (server_running) {
                perror("❌ accept");
            }
            break;
        }
        
        printf("🔗 New client connected: %s:%d (socket %d)\n", 
               inet_ntoa(address.sin_addr), ntohs(address.sin_port), client_socket);
        
        int *client_sock = malloc(sizeof(int));
        *client_sock = client_socket;
        
        if (pthread_create(&thread_id, NULL, client_handler, client_sock) != 0) {
            perror("❌ pthread_create");
            close(client_socket);
            free(client_sock);
        } else {
            pthread_detach(thread_id);
            printf("🧵 Created thread for client handling\n");
        }
    }
    
    printf("🛑 Shutting down server...\n");
    close(server_fd);
    cleanup_redis();
    printf("👋 Server stopped successfully\n");
    
    return 0;
}

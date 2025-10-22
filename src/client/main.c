#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include "../include/client/auth.h"
#include "../include/client/network.h"
#include "../include/client/ui.h"
#include "../include/common/crypto.h"

int main(int argc, char* argv[]) {
    printf("🔐 Secure Messenger Client\n");
    printf("==========================\n");
    
    if (argc < 3) {
        printf("Usage: %s <server_ip> <server_port>\n", argv[0]);
        printf("Example: %s 127.0.0.1 8888\n", argv[0]);
        return 1;
    }
    
    const char* server_ip = argv[1];
    int server_port = atoi(argv[2]);
    
    printf("📍 Connecting to %s:%d\n", server_ip, server_port);
    
    // Initialize cryptography
    printf("🔑 Initializing cryptography...\n");
    if (init_crypto() != 0) {
        fprintf(stderr, "❌ Failed to initialize cryptography\n");
        return 1;
    }
    
    // Connect to server
    printf("🔗 Connecting to server...\n");
    int sock = connect_to_server(server_ip, server_port);
    if (sock < 0) {
        fprintf(stderr, "❌ Failed to connect to server\n");
        cleanup_crypto();
        return 1;
    }
    
    printf("✅ Connected to server %s:%d\n", server_ip, server_port);
    
    // Start authentication process
    printf("🔐 Starting authentication...\n");
    if (!authenticate_with_server(sock)) {
        fprintf(stderr, "❌ Authentication failed\n");
        close(sock);
        cleanup_crypto();
        return 1;
    }
    
    printf("✅ Authentication successful\n");
    
    // Start UI
    printf("💬 Starting messaging interface...\n");
    start_ui(sock);
    
    close(sock);
    cleanup_crypto();
    printf("👋 Client shutdown complete\n");
    return 0;
}

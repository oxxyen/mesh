#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../include/client/ui.h"
#include "../include/client/network.h"
#include "../include/common/protocol.h"

static int ui_running = 1;
static int client_socket = -1;

void* message_receiver(void* arg) {
    Message msg;
    
    printf("🧵 Started message receiver thread\n");
    
    while (ui_running) {
        if (receive_message(client_socket, &msg) == 0) {
            switch (msg.type) {
                case MSG_TYPE_TEXT:
                    printf("\n💬 [%s] %s\n", msg.sender_id, msg.data);
                    break;
                case MSG_TYPE_USER_LIST:
                    printf("\n👥 %s\n", msg.data);
                    break;
                case MSG_TYPE_ACK:
                    printf("\n✅ %s\n", msg.data);
                    break;
                case MSG_TYPE_ERROR:
                    printf("\n❌ Error: %s\n", msg.data);
                    break;
                default:
                    printf("\n📨 Received message type: %d\n", msg.type);
                    break;
            }
            printf("💬 Enter message: ");
            fflush(stdout);
        } else {
            // Brief pause to prevent busy waiting
            sleep(100000); // 100ms
        }
    }
    
    printf("🧵 Message receiver thread finished\n");
    return NULL;
}

void start_ui(int sock) {
    client_socket = sock;
    pthread_t receiver_thread;
    
    printf("\n💬 Secure Messenger UI\n");
    printf("=====================\n");
    printf("Available commands:\n");
    printf("  /users - List online users\n");
    printf("  /exit  - Exit the messenger\n");
    printf("  Type any other text to send as message\n");
    printf("\n");
    
    // Start message receiver thread
    if (pthread_create(&receiver_thread, NULL, message_receiver, NULL) != 0) {
        perror("❌ Failed to create receiver thread");
        return;
    }
    
    char input[1024];
    
    while (ui_running) {
        printf("💬 Enter message: ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        input[strcspn(input, "\n")] = '\0';
        
        if (strlen(input) == 0) {
            continue;
        }
        
        // Handle commands
        if (strcmp(input, "/exit") == 0 || strcmp(input, "/quit") == 0) {
            printf("👋 Exiting...\n");
            ui_running = 0;
            break;
        } else if (strcmp(input, "/users") == 0) {
            Message msg;
            msg.type = MSG_TYPE_GET_USERS;
            msg.timestamp = time(NULL);
            strncpy(msg.sender_id, "client", sizeof(msg.sender_id) - 1);
            strncpy(msg.data, "get_users", sizeof(msg.data) - 1);
            send_message(sock, &msg);
            continue;
        } else if (strncmp(input, "/", 1) == 0) {
            printf("❌ Unknown command: %s\n", input);
            printf("💡 Available commands: /users, /exit\n");
            continue;
        }
        
        // Send as text message
        Message msg;
        msg.type = MSG_TYPE_TEXT;
        msg.timestamp = time(NULL);
        strncpy(msg.sender_id, "client", sizeof(msg.sender_id) - 1);
        strncpy(msg.recipient_id, "all", sizeof(msg.recipient_id) - 1);
        strncpy(msg.data, input, sizeof(msg.data) - 1);
        
        printf("📤 Sending message: %s\n", input);
        
        if (send_message(sock, &msg) != 0) {
            printf("❌ Failed to send message\n");
        } else {
            printf("✅ Message sent successfully\n");
        }
    }
    
    ui_running = 0;
    printf("🛑 Waiting for receiver thread to finish...\n");
    pthread_join(receiver_thread, NULL);
    printf("👋 UI shutdown complete\n");
}

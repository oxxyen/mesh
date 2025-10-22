#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/common/protocol.h"

void get_timestamp_str(char* buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", tm_info);
}

int serialize_message(Message* msg, char* buffer, size_t buffer_size) {
    if (msg == NULL || buffer == NULL) return -1;
    
    int written = snprintf(buffer, buffer_size, 
                         "%d|%s|%s|%ld|%s",
                         msg->type,
                         msg->sender_id,
                         msg->recipient_id,
                         msg->timestamp,
                         msg->data);
    
    return (written > 0 && written < buffer_size) ? 0 : -2;
}

int parse_message(unsigned char* data, Message* msg) {
    if (data == NULL || msg == NULL) return -1;
    
    memset(msg, 0, sizeof(Message));
    
    char* token = strtok((char*)data, "|");
    if (token == NULL) return -2;
    
    msg->type = atoi(token);
    
    token = strtok(NULL, "|");
    if (token) strncpy(msg->sender_id, token, sizeof(msg->sender_id) - 1);
    
    token = strtok(NULL, "|");
    if (token) strncpy(msg->recipient_id, token, sizeof(msg->recipient_id) - 1);
    
    token = strtok(NULL, "|");
    if (token) msg->timestamp = atol(token);
    
    token = strtok(NULL, "");
    if (token) strncpy(msg->data, token, sizeof(msg->data) - 1);
    
    return 0;
}

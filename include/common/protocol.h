#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <time.h>
#include <netinet/in.h>

#define BUFFER_SIZE 4096

typedef enum {
    MSG_TYPE_TEXT = 1,
    MSG_TYPE_FILE = 2,
    MSG_TYPE_REGISTER = 3,
    MSG_TYPE_LOGIN = 4,
    MSG_TYPE_GET_USERS = 5,
    MSG_TYPE_USER_LIST = 6,
    MSG_TYPE_REGISTER_RESPONSE = 7,
    MSG_TYPE_LOGIN_RESPONSE = 8,
    MSG_TYPE_ACK = 9,
    MSG_TYPE_ERROR = 10
} MessageType;

typedef struct {
    MessageType type;
    char sender_id[32];
    char recipient_id[32];
    long timestamp;
    char data[1024];
} Message;

// Protocol functions
int serialize_message(Message* msg, char* buffer, size_t buffer_size);
int parse_message(unsigned char* data, Message* msg);
const char* message_type_to_str(MessageType type);

#endif

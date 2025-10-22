#include <string.h>
#include "../include/common/protocol.h"

// Protocol implementation functions
const char* message_type_to_str(MessageType type) {
    switch (type) {
        case MSG_TYPE_TEXT: return "TEXT";
        case MSG_TYPE_FILE: return "FILE";
        case MSG_TYPE_REGISTER: return "REGISTER";
        case MSG_TYPE_LOGIN: return "LOGIN";
        case MSG_TYPE_GET_USERS: return "GET_USERS";
        case MSG_TYPE_USER_LIST: return "USER_LIST";
        case MSG_TYPE_ACK: return "ACK";
        case MSG_TYPE_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

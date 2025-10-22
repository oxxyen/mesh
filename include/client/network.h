#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include "../../include/common/protocol.h"

int connect_to_server(const char* ip, int port);
int send_message(int sock, Message* msg);
int receive_message(int sock, Message* msg);

#endif

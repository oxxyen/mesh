#ifndef MESSAGE_PROCESSOR_H
#define MESSAGE_PROCESSOR_H

#include "../../include/common/protocol.h"

void handle_message(int client_socket, Message* msg);
void handle_registration(int client_socket, Message* msg);
void handle_login(int client_socket, Message* msg);
void handle_text_message(int client_socket, Message* msg);
void handle_file_message(int client_socket, Message* msg);
void handle_get_users(int client_socket, Message* msg);
int process_client_message(int client_socket, unsigned char* encrypted_data, size_t data_len, Message* output_msg);
int send_encrypted_message(int client_socket, Message* msg);

#endif

#ifndef CLIENT_AUTH_H
#define CLIENT_AUTH_H

int authenticate_with_server(int sock);
int register_user(int sock, const char* username);
int login_user(int sock, const char* username);

#endif

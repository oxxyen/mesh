#ifndef AUTH_H
#define AUTH_H

#include <stdlib.h>
#include <netinet/in.h>

typedef struct {
    char user_id[32];
    char username[64];
    char ip_address[INET_ADDRSTRLEN];
    unsigned char public_key[512];
    size_t public_key_len;
} User;

int register_user(const char* username, const char* ip_address, unsigned char* public_key, size_t key_len);
int authenticate_user(const char* username, const char* challenge, const char* response);
int generate_auth_challenge(const char* username, char* challenge, size_t challenge_len);

#endif

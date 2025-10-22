#ifndef REDIS_H
#define REDIS_H

#include "../../include/server/auth.h"

int init_redis();
void cleanup_redis();
int redis_store_user(const char* user_id, const char* username, const char* ip_address, 
                    unsigned char* public_key, size_t key_len);
int redis_user_exists(const char* username);
int redis_get_user(const char* username, User* user);
int redis_store_message(const char* sender, const char* recipient, const char* message, long timestamp);
int redis_store_challenge(const char* username, const char* challenge);

#endif

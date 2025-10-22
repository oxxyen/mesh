#include <hiredis/hiredis.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../include/server/redis.h"

static redisContext* redis_ctx = NULL;

int init_redis() {
    printf("🔗 Initializing Redis connection...\n");
    
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    redis_ctx = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
    
    if (redis_ctx == NULL || redis_ctx->err) {
        if (redis_ctx) {
            fprintf(stderr, "❌ Redis connection error: %s\n", redis_ctx->errstr);
            redisFree(redis_ctx);
        } else {
            fprintf(stderr, "❌ Cannot allocate Redis context\n");
        }
        return -1;
    }
    
    // Test the connection
    redisReply* reply = redisCommand(redis_ctx, "PING");
    if (reply == NULL) {
        fprintf(stderr, "❌ Redis PING failed\n");
        redisFree(redis_ctx);
        redis_ctx = NULL;
        return -1;
    }
    
    printf("✅ Redis PING: %s\n", reply->str);
    freeReplyObject(reply);
    
    printf("✅ Connected to Redis successfully\n");
    return 0;
}

void cleanup_redis() {
    if (redis_ctx) {
        printf("🔌 Disconnecting from Redis...\n");
        redisFree(redis_ctx);
        redis_ctx = NULL;
        printf("✅ Redis connection closed\n");
    }
}

int redis_store_user(const char* user_id, const char* username, const char* ip_address, 
                    unsigned char* public_key, size_t key_len) {
    if (redis_ctx == NULL) {
        printf("❌ Redis not initialized\n");
        return -1;
    }
    
    printf("💾 Storing user in Redis: %s (%s)\n", username, user_id);
    
    char key[128];
    snprintf(key, sizeof(key), "user:%s", username);
    
    // Store user data in hash
    redisReply* reply = redisCommand(redis_ctx,
        "HMSET %s user_id %s username %s ip %s public_key %b",
        key, user_id, username, ip_address, public_key, key_len);
    
    if (reply == NULL) {
        printf("❌ Redis command failed for user storage\n");
        return -2;
    }
    
    int result = 0;
    if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0) {
        printf("✅ User %s stored successfully\n", username);
    } else {
        printf("❌ Failed to store user %s\n", username);
        result = -3;
    }
    freeReplyObject(reply);
    
    // Add to users set
    reply = redisCommand(redis_ctx, "SADD users %s", username);
    if (reply) {
        printf("✅ Added %s to users set\n", username);
        freeReplyObject(reply);
    }
    
    return result;
}

int redis_user_exists(const char* username) {
    if (redis_ctx == NULL) {
        printf("❌ Redis not initialized\n");
        return -1;
    }
    
    char key[128];
    snprintf(key, sizeof(key), "user:%s", username);
    
    redisReply* reply = redisCommand(redis_ctx, "EXISTS %s", key);
    if (reply == NULL) {
        printf("❌ Redis EXISTS command failed\n");
        return -1;
    }
    
    int exists = (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) ? 1 : 0;
    freeReplyObject(reply);
    
    printf("🔍 User %s exists: %s\n", username, exists ? "YES" : "NO");
    return exists;
}

int redis_get_user(const char* username, User* user) {
    if (redis_ctx == NULL || user == NULL) {
        printf("❌ Redis not initialized or invalid user pointer\n");
        return -1;
    }
    
    printf("🔍 Retrieving user from Redis: %s\n", username);
    
    char key[128];
    snprintf(key, sizeof(key), "user:%s", username);
    
    redisReply* reply = redisCommand(redis_ctx, "HGETALL %s", key);
    if (reply == NULL || reply->type != REDIS_REPLY_ARRAY) {
        printf("❌ Failed to get user %s from Redis\n", username);
        if (reply) freeReplyObject(reply);
        return -2;
    }
    
    // Initialize user structure
    memset(user, 0, sizeof(User));
    
    // Parse hash fields
    int fields_found = 0;
    for (size_t i = 0; i < reply->elements; i += 2) {
        const char* field = reply->element[i]->str;
        const char* value = reply->element[i+1]->str;
        
        if (strcmp(field, "user_id") == 0) {
            strncpy(user->user_id, value, sizeof(user->user_id) - 1);
            fields_found++;
        } else if (strcmp(field, "username") == 0) {
            strncpy(user->username, value, sizeof(user->username) - 1);
            fields_found++;
        } else if (strcmp(field, "ip") == 0) {
            strncpy(user->ip_address, value, sizeof(user->ip_address) - 1);
            fields_found++;
        }
    }
    
    freeReplyObject(reply);
    
    if (fields_found >= 2) {
        printf("✅ Retrieved user %s successfully\n", username);
        return 0;
    } else {
        printf("❌ Incomplete user data for %s\n", username);
        return -3;
    }
}

int redis_store_message(const char* sender, const char* recipient, const char* message, long timestamp) {
    if (redis_ctx == NULL) {
        printf("❌ Redis not initialized\n");
        return -1;
    }
    
    printf("💾 Storing message from %s to %s\n", sender, recipient);
    
    char key[128];
    snprintf(key, sizeof(key), "msg:%s:%s:%ld", sender, recipient, timestamp);
    
    redisReply* reply = redisCommand(redis_ctx, "SET %s %s", key, message);
    if (reply == NULL) {
        printf("❌ Failed to store message in Redis\n");
        return -2;
    }
    
    int result = 0;
    if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0) {
        printf("✅ Message stored successfully\n");
    } else {
        printf("❌ Failed to store message\n");
        result = -3;
    }
    freeReplyObject(reply);
    
    // Add to recipient's message list
    char list_key[128];
    snprintf(list_key, sizeof(list_key), "messages:%s", recipient);
    reply = redisCommand(redis_ctx, "LPUSH %s %s", list_key, key);
    if (reply) {
        printf("✅ Added message to %s's list\n", recipient);
        freeReplyObject(reply);
    }
    
    return result;
}

int redis_store_challenge(const char* username, const char* challenge) {
    if (redis_ctx == NULL) {
        printf("❌ Redis not initialized\n");
        return -1;
    }
    
    printf("🔐 Storing challenge for %s\n", username);
    
    char key[128];
    snprintf(key, sizeof(key), "challenge:%s", username);
    
    redisReply* reply = redisCommand(redis_ctx, "SETEX %s 300 %s", key, challenge); // 5 minutes
    if (reply == NULL) {
        printf("❌ Failed to store challenge in Redis\n");
        return -2;
    }
    
    int result = (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0) ? 0 : -3;
    freeReplyObject(reply);
    
    printf("✅ Challenge stored for %s: %s\n", username, challenge);
    return result;
}

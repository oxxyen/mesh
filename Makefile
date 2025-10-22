# Secure Messenger Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2 -g
LIBS = -lhiredis -lssl -lcrypto -lpthread
INCLUDES = -I./include -I./include/common -I./include/server -I./include/client

SERVER_SRC = src/server/main.c src/server/auth.c src/server/message_processor.c src/server/crypto.c src/server/redis.c
CLIENT_SRC = src/client/main.c src/client/auth.c src/client/network.c src/client/crypto.c src/client/ui.c
COMMON_SRC = src/common/utils.c src/common/protocol.c

SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
COMMON_OBJ = $(COMMON_SRC:.c=.o)

all: server client

server: $(SERVER_OBJ) $(COMMON_OBJ)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/messenger_server $^ $(LIBS)

client: $(CLIENT_OBJ) $(COMMON_OBJ)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/messenger_client $^ $(LIBS) -lncurses

%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

install-deps:
	sudo apt-get update
	sudo apt-get install -y redis-server libhiredis-dev libssl-dev libncurses5-dev net-tools

setup-redis:
	sudo systemctl start redis-server
	redis-cli ping

clean:
	rm -f src/server/*.o src/client/*.o src/common/*.o bin/messenger_server bin/messenger_client

test-server: server
	./bin/messenger_server

test-client: client
	./bin/messenger_client 127.0.0.1 8888

.PHONY: all clean install-deps setup-redis test-server test-client

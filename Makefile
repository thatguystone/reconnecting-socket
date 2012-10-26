CC = gcc
CFLAGS += -std=gnu99 -Wall -Woverride-init -Wsign-compare -Wtype-limits -Wuninitialized

.PHONY: test

all: persistent_socket.o

test:
	$(CC) $(CFLAGS) persistent_socket.o test.c -o $@
	./test

persistent_socket.o: persistent_socket.c persistent_socket.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@
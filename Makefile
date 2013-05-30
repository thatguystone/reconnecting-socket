CC = gcc
CFLAGS += -g -std=gnu99 -Wall -Woverride-init -Wsign-compare -Wtype-limits -Wuninitialized

.PHONY: test

all: reconnecting_socket.o

test: reconnecting_socket.c reconnecting_socket.h test.c
	$(CC) $(CFLAGS) test.c -o $@
	./test
	valgrind \
		--tool=memcheck \
		--leak-check=full \
		--leak-resolution=high \
		--num-callers=20 \
		./test

clean:
	rm -f reconnecting_socket.o
	rm -f test

reconnecting_socket.o: reconnecting_socket.c reconnecting_socket.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

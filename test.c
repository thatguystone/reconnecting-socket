#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "persistent_socket.h"

#define HOST "localhost"
#define PORT 45673

#define TEST(expr) \
	total++; \
	if (!(expr)) { \
		failed++; \
		printf("Fail (line %d): %s\n", __LINE__, #expr); \
		return; \
	}

#define ACCEPT(client) { \
	client = accept(server, NULL, NULL); \
	TEST(client != -1); \
	fcntl(client, F_SETFL, O_NONBLOCK);}

/**
 * The signature for all test functions.
 *
 * @param psocket The socket that was opened to the server.
 */
typedef void (*test_fn)(psocket *psocket);

/**
 * The socket the server is listening on.
 */
static int server = 0;

/**
 * Counters for tests.
 */
static unsigned int total = 0;
static unsigned int failed = 0;

void test_sane(psocket *psocket) {
	// Just make sure a connection comes through
	int client;
	ACCEPT(client);
}

void test_send(psocket *psocket) {
	int client;
	ACCEPT(client);
	
	psocket_send(psocket, "test", 4);
	
	char buff[128];
	memset(buff, 0, sizeof(buff));
	TEST(read(client, buff, sizeof(buff)) == 4);
	TEST(strcmp(buff, "test") == 0);
}

void test_send_reconnect(psocket *psocket) {
	int client;
	ACCEPT(client);
	
	close(client);
	
	// Make sure the other side knows it's closed
	for (int i = 0; i < 2; i++) {
		psocket_send(psocket, "test", 4);
	}
	
	ACCEPT(client);
	psocket_send(psocket, "test", 4);
	
	char buff[128];
	memset(buff, 0, sizeof(buff));
	TEST(read(client, buff, sizeof(buff)) == 4);
	TEST(strcmp(buff, "test") == 0);
}

void test_read(psocket *psocket) {
	int client;
	ACCEPT(client);
	
	send(client, "test", 4, MSG_NOSIGNAL);
	
	char buff[5];
	memset(buff, 0, sizeof(buff));
	
	TEST(psocket_read(psocket, buff, sizeof(buff)-1) == 4);
	TEST(strcmp(buff, "test") == 0);
}

void test_read_reconnect(psocket *psocket) {
	int client;
	ACCEPT(client);
	close(client);
	
	char buff[5];
	
	TEST(psocket_read(psocket, buff, sizeof(buff)-1) == 0);
	
	ACCEPT(client);
	send(client, "test", 4, MSG_NOSIGNAL);
	
	memset(buff, 0, sizeof(buff));
	TEST(psocket_read(psocket, buff, sizeof(buff)-1) == 4);
	TEST(strcmp(buff, "test") == 0);
}

void setup_server() {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	char port[6];
	snprintf(port, sizeof(port), "%d", PORT);
	
	struct addrinfo *res;
	int ret;
	if ((ret = getaddrinfo(HOST, port, &hints, &res)) != 0) {
		fprintf(stderr, "test getaddrinfo: %s\n", gai_strerror(ret));
		exit(1);
	}
	
	if ((server = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		perror("socket");
		exit(1);
	}
	
	int on = 1;
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	
	if (fcntl(server, F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl");
		exit(1);
	}
	
	if (bind(server, res->ai_addr, res->ai_addrlen) == -1) {
		perror("bind");
		exit(1);
	}
	
	if (listen(server, 100) == -1) {
		perror("listen");
		exit(1);
	}
	
	freeaddrinfo(res);
}

void test(test_fn fn) {
	psocket *socket;
	TEST(psocket_connect(HOST, PORT, &socket) == 0);
	
	fn(socket);
	
	psocket_close(socket);
}

int main(int argc, char **argv) {
	setup_server();
	
	printf("Running tests:\n\n");
	
	test(test_sane);
	test(test_send);
	test(test_send_reconnect);
	test(test_read);
	test(test_read_reconnect);
	
	printf("\nResults: %u total; %u failing\n", total, failed);
	return failed != 0;
}
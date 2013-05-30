/**
 * Provides a TCP socket connection to an address that is persistent. When there is a
 * disconnect or error, this transparently reconnects on the next socket operation
 * and keeps on going as though nothing went wrong. All sockets used are non-blocking.
 *
 * By its very nature, persistent sockets hide errors, so the functions don't
 * typically return errors. This is essentially a UDP socket that has guaranteed delivery
 * while connected.
 *
 * @file reconnecting_socket.h
 * @author Andrew Stone <andrew@clovar.com>
 * @copyright 2012 Andrew Stone
 *
 * @internal This file is part of persistent-socket and is released under the
 * MIT License: http://opensource.org/licenses/MIT
 */

/**
 * Everything behind the reconnecting socket
 */
typedef struct rsocket rsocket_t;

/**
 * Create a new connection.
 * 
 * @param host The hostname to connect to.
 * @param port The port to connect to.
 * @param[out] rsocket Where the new persistent socket should be put.
 *
 * @return 0 on success.
 * @return -1 on address lookup error.
 */
int rsocket_connect(const char *host, const int port, rsocket_t **rsocket);

/**
 * Closes a rsocket connection.
 *
 * @param rsocket The persistent socket to close.
 */
void rsocket_close(rsocket_t *rsocket);

/**
 * Send some data out to the server.
 *
 * @param rsocket The persistent socket
 * @param msg The message to send
 * @param len The length of the message
 */
void rsocket_send(rsocket_t *rsocket, char *msg, int len);

/**
 * Read data from the server.
 *
 * @param rsocket The socket to read from
 * @param buff Where the data should be put
 * @param len The length of the data buffer
 *
 * @return The length of data read from the socket, 0 or greater.
 */
int rsocket_read(rsocket_t *rsocket, char *buff, size_t len);

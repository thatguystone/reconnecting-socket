# Persistent Socket

A persistent socket is a TCP connection to a server that has all the guarantees of UDP (aka. none). This type of socket is for the rare case when you need a connection to a server, but you don't particularly care if the data you send gets there or not, but at the same time, you want high uptime. This was created for that spot in between TCP and UDP that is hard to scratch.

Originally, I needed a way to talk to [Graphite](http://graphite.wikidot.com/).  Though it does support UDP, I noticed that, at high load times in EC2, ~20% of the packets were being dropped.  Technically, this is acceptable, but it just produced some terrible results, and you have to change Carbon's configuration in ways that aren't recommended to get it to work.  Enter stage left: persistent socket.

## Example

Using a persistent socket is dead simple.

```c
psocket *socket;
if (psocket_connect("some.host.com", 1234, &socket) != 0) {
	// Some error occurred
	return;
}

// Send some data to the socket: there are no error conditions here,
// so send it and forget it
psocket_send(socket, "test", 4);

// Read data from a socket: there are no error conditions here, you either
// read some data, or you didn't
char buff[32];
memset(buff, 0, sizeof(buff));
if (psocket_read(socket, buff, sizeof(buff)-1) > 0) {
	// Got data
} else {
	// Didn't get any data
}

// Shut down the socket and free all the memory used
psocket_close(socket);
```

## Features

* From a hostname and port, start sending data
* If there are multiple addresses for a hostname, it goes through all while trying to reconnect
* If a connection is ever lost, it transparently reconnects (though data will be lost)
* Treat a TCP socket like a UDP socket, without any of the pain
* Non-blocking sockets, so you can always go about your day

## Supported Platforms

* Linux (maybe other *NIXs)
#include <stdio.h>
#include <string.h>

/**
* memset(void *s, int c, size_t n)
**/

#include <stdlib.h>

/**
* #define EXIT_FAILURE
**/

#include <sys/socket.h>

/**
* socket(int domain, int type, int protocol)
* connect(int socket, const struct sockaddr *address, socklen_t addr_len)
* shutdown(int socket, int how)
**/

#include <unistd.h>

/**
* read(int fd, void *buf, size_t bytes)
* write(int fd, void *buf, size_t bytes)
* close(int fd)
**/

#include <arpa/inet.h>

/**
* needs:
** <sys/socket.h>
** <netinet/in.h>
* htons(uint32_t hostlong)
**/

#include <netinet/in.h>
/**
* struct sockaddr_in {
	sa_family_t sin_family; // famille d'adresse : AF_INET
	u_int16_t sin_port; // port dans ordre des octets réseaux
	struct in_addr sin_addr; // adresse internet
* }
*
* struct in_addr {
	u_int32_t s_addr; // adresse respectant boutiste réseau
* }
**/

/**
* create a socket
* bind it (give it a "name")
* be in a listening position
* accept connections
* shutdown connections
* close sockets
**/

#define SERVER_PORT	10000
#define SERVER_HOST	"127.0.0.1"

int main(int ac, char **av)
{
	int client_socket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client_socket_name;

	if (ac < 1) {
		printf("Usage:\n\t./client [message]\n");
		exit(1);
	}
	memset((char *) &client_socket_name, 0, sizeof(struct sockaddr_in));
	client_socket_name.sin_family = AF_INET;
	client_socket_name.sin_port = htons(SERVER_PORT);
	client_socket_name.sin_addr.s_addr= htonl(INADDR_LOOPBACK);

	connect(client_socket, (struct sockaddr *) &client_socket_name,
		sizeof(struct sockaddr_in));
	write(client_socket, av[1], strlen(av[1]));
	write(client_socket, "\n", 1);
	shutdown(client_socket, 2);
	close(client_socket);
}

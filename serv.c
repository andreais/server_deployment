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
* bind(int socket, const struct sockaddr* address, socklen_t address_len)
* listen(int sockfd, int backlog)
* accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len)
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
* inet_aton(const char *cp, struct in_addr *inp)
* htons(uint16_t hostshort)
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

#define LOCAL_PORT 10000
#define LOCAL_HOST 127.0.0.1

int main(void)
{
	int server_socket = socket(PF_INET, SOCK_STREAM, 0); // opening a socket
	struct sockaddr_in server_socket_name; // struct containing information about the socket
	int client_socket; // client's socket
	int optval = 1;

	if (server_socket < 0)
		return EXIT_FAILURE;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,&optval, sizeof(int)); // make socket reuseable after closing server
	memset((char *) &server_socket_name, 0, sizeof(struct sockaddr_in)); // giving enough memory to the struct sockaddr_in
	server_socket_name.sin_family = AF_INET;
	server_socket_name.sin_port = htons(LOCAL_PORT); // converting port
	bind(server_socket, (struct sockaddr *) &server_socket_name, sizeof(struct sockaddr_in)); // binding socket
	server_socket_name.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	return 0;
}

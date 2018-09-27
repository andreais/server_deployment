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

int main(void)
{
	return 0;
}

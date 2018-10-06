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

#define	BOLD		"\e[1m"
#define DEFAULT		"\e[0m"

#define SERVER_PORT	10000
//#define SERVER_HOST	"206.189.26.6"
#define SERVER_HOST	"127.0.0.1"

#include <stdlib.h>
#include <unistd.h>

int main(int ac, char **av)
{
	int client_socket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client_socket_name;
	char buff[256];
	char *nickname;

	if (ac == 1) {
		printf("Usage:\n\t./client [nickname]\n");
		return 2;
	}
	nickname = malloc(sizeof(char) * strlen(av[1]) + 2);
	strcpy(nickname, av[1]);
	strcat(nickname, "\n");
	memset((char *) &client_socket_name, 0, sizeof(struct sockaddr_in));
	client_socket_name.sin_family = AF_INET;
	client_socket_name.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_HOST, &client_socket_name.sin_addr);
	connect(client_socket, (struct sockaddr *) &client_socket_name,
		sizeof(struct sockaddr_in));
	write(client_socket, nickname, strlen(nickname));
	free(nickname);
	while ((fgets(buff, sizeof(buff), stdin)) != NULL) {
		write(client_socket, buff, strlen(buff));
	}
	shutdown(client_socket, 2);
	close(client_socket);
}

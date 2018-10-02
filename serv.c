#include <stdio.h>
#include <sys/wait.h>
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
#define LOCAL_HOST "127.0.0.1"

#define GREEN		"\e[32m"
#define DEFAULT		"\e[0m"
#define BOLD		"\e[1m"

void creating_client(int client_socket, struct sockaddr_in client_socket_name)
{
	char *buff = malloc(sizeof(char) * 1);

	if (client_socket > 0) {
		printf("%s%s%s successfully connected\n", BOLD, inet_ntoa(client_socket_name.sin_addr), DEFAULT);
		while (read(client_socket, buff, 1)) {
			write(1, &buff[0], 1);
		}
	}
	free(buff);
}

void wait_connections(int server_socket)
{
	pid_t pid = fork();
	int client_socket;
	struct sockaddr_in client_socket_name;
	unsigned int addr_len = sizeof(struct sockaddr_in);


	if (pid == 0) {
		client_socket = accept(server_socket, (struct sockaddr *) &client_socket_name, &addr_len);
		creating_client(client_socket, client_socket_name);
		shutdown(client_socket, 2);
		close(client_socket);
	} else {
		client_socket = accept(server_socket, (struct sockaddr *) &client_socket_name, &addr_len);
		creating_client(client_socket, client_socket_name);
		shutdown(client_socket, 2);
		close(client_socket);
	}
}

int main(void)
{
	int server_socket = socket(PF_INET, SOCK_STREAM, 0); // opening a socket
	struct sockaddr_in server_socket_name; // struct containing information about the socket
	int optval = 1;
	if (server_socket < 0)
		return EXIT_FAILURE;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,&optval, sizeof(int)); // make socket reuseable after closing server
	memset((char *) &server_socket_name, 0, sizeof(struct sockaddr_in)); // giving enough memory to the struct sockaddr_in
	server_socket_name.sin_family = AF_INET;
	server_socket_name.sin_port = htons(LOCAL_PORT); // converting port
	bind(server_socket, (struct sockaddr *) &server_socket_name, sizeof(struct sockaddr_in)); // binding socket
	server_socket_name.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	listen(server_socket, 3); // (see the second argument, that is the most important here)

	printf("%s%sServer created.%s\n", GREEN, BOLD, DEFAULT);
	printf("%sListening on:\t%s:%d%s\n", GREEN, LOCAL_HOST, LOCAL_PORT, DEFAULT);

	// TO FORK
	// accept() IS A BLOCKING FUNCTION
	while (1) {
		wait_connections(server_socket);
		printf("Connection lost\n");
	}
	shutdown(server_socket, 2);
	close(server_socket);
	return 0;
}

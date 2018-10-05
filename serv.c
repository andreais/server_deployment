#include <stdio.h>
#include <signal.h>
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

#include <stdbool.h>

static bool closing = false;

void creating_client(int client_socket, struct sockaddr_in client_socket_name, FILE* logfile)
{
	/**
	* creating_client is more like, printing texts and will be used to send too
	* its a generic function to get input from a file descriptor (client_socket) and pretty print it
	*/
	char buff[256];

	if (client_socket > 0) {
		fprintf(logfile, "%s successfully connected\n", inet_ntoa(client_socket_name.sin_addr));
		printf("%s%s%s successfully connected\n", BOLD, inet_ntoa(client_socket_name.sin_addr), DEFAULT);
		while (fgets(buff, sizeof(buff), fdopen(client_socket, "r"))) {
			fprintf(logfile, "%s: %s", inet_ntoa(client_socket_name.sin_addr), buff);
			printf("%s%s%s: ", BOLD, inet_ntoa(client_socket_name.sin_addr), DEFAULT);
			printf("%s", buff);
		}
	}
}

void wait_connections(int server_socket, FILE* logfile)
{
	pid_t pid = fork();
	int client_socket;
	struct sockaddr_in client_socket_name; // struct containing informations on the client_socket
	unsigned int addr_len = sizeof(struct sockaddr_in); // well, think about it yourself

	if (pid > 0) {
		if (closing == true) {
			kill(pid, SIGKILL);
			return;
		}
		client_socket = accept(server_socket, (struct sockaddr*) &client_socket_name, &addr_len);
		if ((fork()) != 0)
			wait_connections(server_socket, logfile);
		creating_client(client_socket, client_socket_name, logfile);
		fprintf(logfile, "Connection lost from %s\n", inet_ntoa(client_socket_name.sin_addr));
		printf("Connection lost from %s%s%s\n", BOLD, inet_ntoa(client_socket_name.sin_addr), DEFAULT);
		shutdown(client_socket, 2);
		close(client_socket);
	} else {
		if (closing == true)
			return;
		client_socket = accept(server_socket, (struct sockaddr*) &client_socket_name, &addr_len);
		if ((fork()) != 0)
			wait_connections(server_socket, logfile);
		creating_client(client_socket, client_socket_name, logfile);
		fprintf(logfile, "Connection lost from %s\n", inet_ntoa(client_socket_name.sin_addr));
		printf("Connection lost from %s%s%s\n", BOLD, inet_ntoa(client_socket_name.sin_addr), DEFAULT);
		shutdown(client_socket, 2);
		close(client_socket);
	}
}

void reading_input(int server_socket, FILE* logfile)
{
	char input[256];

	while (fgets(input, sizeof(input), stdin)) {
		if (strcmp(input, "!q\n") == 0) {
			// STOP PROPERLY AND SEND QUIT MESSAGE, + WAITING FOR RESPONSE
			shutdown(server_socket, 2); // stopping server_socket
			close(server_socket); // closing server_socket
			fclose(logfile);
			closing = true;
			exit(1);
		}
	}
}

int main(void)
{
	int server_socket = socket(PF_INET, SOCK_STREAM, 0); // opening a socket
	struct sockaddr_in server_socket_name; // struct containing information about the socket
	int optval = 1;
	FILE *logfile = fopen("logs.txt", "w");
	pid_t pid;

	if (!logfile)
		return 1;
	if (server_socket < 0)
		return EXIT_FAILURE;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,&optval, sizeof(int)); // make socket reuseable after closing server
	memset((char *) &server_socket_name, 0, sizeof(struct sockaddr_in)); // giving enough memory to the struct sockaddr_in
	server_socket_name.sin_family = AF_INET;
	server_socket_name.sin_port = htons(LOCAL_PORT); // converting port
	bind(server_socket, (struct sockaddr *) &server_socket_name, sizeof(struct sockaddr_in)); // binding socket
	inet_aton(LOCAL_HOST, &server_socket_name.sin_addr); // listening on the local host (127.0.0.1)
	listen(server_socket, 3); // (see the second argument, that is the most important here)
	fprintf(logfile, "Server created\n");
	printf("%s%sServer created.%s\n", GREEN, BOLD, DEFAULT);
	fprintf(logfile, "Listening on:\t%s:%d\n", LOCAL_HOST, LOCAL_PORT);
	printf("%sListening on:\t%s:%d%s\n", GREEN, LOCAL_HOST, LOCAL_PORT, DEFAULT);
	pid = fork();
	while (!closing) {
		if (pid == 0 && closing != true)
			wait_connections(server_socket, logfile);
		else
			reading_input(server_socket, logfile);
	}
	return 0;
}

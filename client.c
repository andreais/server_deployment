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
#define SERVER_HOST	"127.0.0.1"

#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <ncurses.h>
#include "client.h"

void *poll_events(void *vargp)
{
	struct pollfd fds[2];
	int ret;
	struct tid_arg *args = vargp;
	char *buff = NULL;
	size_t s_buff = 256;
	char stop_msg[] = "STOPPING\n";
	FILE *file_fd;
	FILE *file_fd_client;

	fds[0].fd = args->fd;
	fds[0].events = POLLIN;
	fds[1].fd = args->pipeCP[0];
	fds[1].events = POLLIN;
	file_fd = fdopen(fds[0].fd, "r");
	file_fd_client = fdopen(fds[1].fd, "r");
	while (*args->stop_server == 0) {
		ret = poll(fds, 2, 1000);
		if (ret == 0)
			continue;
		else if (ret > 0) {
			if (fds[0].revents & POLLIN) {
				getline(&buff, &s_buff, file_fd);
				if (strcmp(buff, "CONNECTED\n") == 0) {
					write(1, buff, strlen(buff));
					dprintf(fds[0].fd, "%s\n", args->nickname);
				} else if (strncmp(buff, stop_msg, sizeof(stop_msg)) == 0) {
					printf("Server has stopped.\nPress enter to quit.\n");
					*args->stop_server = 1;
					free(buff);
					return NULL;
				} else {
					write(1, buff, strlen(buff));
				}
			}
			if (fds[1].revents & POLLIN) {
				if (getline(&buff, &s_buff, file_fd_client)) {
					if (strncmp(buff, "!q", 2) == 0) {
						*args->stop_server = 1;
						printf("PRESS ENTER AGAIN\n");
						free(buff);
						return NULL;
					}
					dprintf(fds[0].fd, "%s", buff);
				}
				else
					return NULL;
			}
		}
	}
	return NULL;
}

int read_streams(int socket, char const *nickname)
{
	int pipeCP[2];
	char *input = NULL;
	size_t sizen = 256;
	pthread_t tid;
	struct tid_arg args;
	
	if (pipe(pipeCP) < 0)
		return FAILED_PIPE;
	// thread
	args.fd = socket;
	args.pipeCP = malloc(sizeof(int) * 2);
	if (!args.pipeCP)
		return FAILED_ALLOC;
	args.pipeCP[0] = pipeCP[0];
	args.pipeCP[1] = pipeCP[1];
	args.nickname = nickname;
	args.stop_server = malloc(sizeof(int));
	*args.stop_server = 0;
	if (pthread_create(&tid, NULL, poll_events, &args) != 0)
		return FAILED_THREAD_CREATION;
	while (*args.stop_server == 0 && getline(&input, &sizen, stdin) != EOF) {
		write(pipeCP[1], input, strlen(input));
	}
	write(pipeCP[1], "\0", 1);
	pthread_join(tid, NULL);
	free(args.pipeCP);
	free(args.stop_server);
	free(input);
	return 0;
}

int main(int ac, char **av)
{
	int return_value = 0;
	int client_socket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client_socket_name;

	if (client_socket == -1)
		return FAILED_CLIENT_SOCKET;
	if (ac == 1) {
		printf("Usage:\n\t./client [nickname]\n");
		return NE_ARGS;
	}
	memset(&client_socket_name, 0, sizeof(struct sockaddr_in));
	client_socket_name.sin_family = AF_INET;
	client_socket_name.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_HOST, &client_socket_name.sin_addr);
	if ((connect(client_socket, (struct sockaddr *) &client_socket_name,
		sizeof(struct sockaddr_in))) == -1)
		return FAILED_CONNECT;
	return_value = read_streams(client_socket, av[1]);
	shutdown(client_socket, 2);
	close(client_socket);
	return return_value;
}

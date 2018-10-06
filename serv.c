#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <fcntl.h>
#include <poll.h>

#include "serv.h"

void reading_input(void)
{
	char input[256];

	while (fgets(input, sizeof(input), stdin)) {
		if (strcmp(input, "!q\n") == 0) {
			return;
		}
	}
}

void push_tmp(poll_collector *socket, client_socket const *tmp)
{
	socket->fds_n++;
	if (!(realloc(socket, sizeof(poll_collector) * socket->fds_n)))
		exit(EXIT_FAILURE);
	socket->fds[socket->fds_n - 2].fd = tmp->fd;
	socket->name[socket->fds_n - 2] = strdup(inet_ntoa(tmp->socket_name.sin_addr));
}

void wait_connections(int server_socket)
{
	struct pollfd fds[1];
	client_socket tmp;
	int ret;

	fds[0].fd = server_socket;
	fds[0].events = POLLIN;
	while (1) {
		ret = poll(fds, 1, 5);
		if (fds[0].revents & POLLIN) {
			printf("OK\n");
		}
	}
}

int main(void)
{
	int server_socket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_socket_name;
	int optval = 1;
	FILE *logfile = fopen("logs.txt", "w");
	pid_t pid;

	if (server_socket < 0)
		return EXIT_FAILURE;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,&optval, sizeof(int));
	memset((char *) &server_socket_name, 0, sizeof(struct sockaddr_in));
	server_socket_name.sin_family = AF_INET;
	server_socket_name.sin_port = htons(LOCAL_PORT); // converting port
	bind(server_socket, (struct sockaddr *) &server_socket_name, sizeof(struct sockaddr_in)); 
	inet_aton(LOCAL_HOST, &server_socket_name.sin_addr); 
	listen(server_socket, 3);
	printf("%s%sServer created.%s\n", GREEN, BOLD, DEFAULT);
	printf("%sListening on:\t%s:%d%s\n", GREEN, LOCAL_HOST, LOCAL_PORT, DEFAULT);

	pid = fork();
	if (pid == 0) {
		wait_connections(server_socket);
	} else {
		reading_input();
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		shutdown(server_socket, 2); // stopping server_socket
		close(server_socket); // closing server_socket
		fclose(logfile);
		exit(1);
	}
	return 0;
}

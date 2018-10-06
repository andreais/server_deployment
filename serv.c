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

poll_collector create_poll(int server_socket)
{
	poll_collector sockets;

	sockets.fds = malloc(sizeof(struct pollfd) * 1);
	sockets.fds_n = 1;
	sockets.name = malloc(sizeof(char *) * 1);
	sockets.fds[0].fd = server_socket;
	sockets.fds[0].events = POLLIN;
	sockets.name[0] = strdup("SERVER");
	return sockets;
}

void push_back(poll_collector *sockets, client_socket const *tmp)
{
	sockets->fds = realloc(sockets->fds, sizeof(struct pollfd) * (sockets->fds_n + 1));
	sockets->name = realloc(sockets->name, sizeof(char *) * (sockets->fds_n + 1));
	sockets->fds_n++;
	sockets->fds[sockets->fds_n - 1].fd = tmp->fd;
	sockets->fds[sockets->fds_n - 1].events = POLLIN;
	sockets->name[sockets->fds_n - 1] = strdup(inet_ntoa(tmp->socket_name.sin_addr));

}

void read_text(poll_collector *sockets, int index)
{
	char buff[256] = {'\0'};
	int ret;

	ret = recv(sockets->fds[index].fd, buff, sizeof(buff), 0);
	if (ret == 0) {
		shutdown(sockets->fds[index].fd, 2);
		close(sockets->fds[index].fd);
		sockets->fds_n--;
		printf("%s DISCONNECTED\n", sockets->name[index]);
		return;
	} else
		printf("%s SAYS: %s", sockets->name[index], buff);
}

void find_socket(poll_collector *sockets)
{
	for (unsigned int i = 1; i < sockets->fds_n; i++) {
		if (sockets->fds[i].revents & POLLIN) {
			printf("GOT DATA FROM %s\n", sockets->name[i]);
			read_text(sockets, i);
		}
	}
}

void wait_connections(int server_socket)
{
	poll_collector sockets = create_poll(server_socket);
	client_socket tmp = {0};
	int ret;

	tmp.addr_len = sizeof(struct sockaddr_in);
	while (1) {
		ret = poll(sockets.fds, sockets.fds_n, 5);
		if (ret == -1)
			exit(1);
		if (ret > 0)
			find_socket(&sockets);
		if (sockets.fds[0].revents & POLLIN) {
			tmp.fd = accept(server_socket, (struct sockaddr *) &tmp.socket_name, &tmp.addr_len);
			push_back(&sockets, &tmp);
			printf("NEW CONNECTION\n");
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

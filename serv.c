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

	// that's so shit lol
	// TODO: make something of this so called function
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
	sockets.first_data = malloc(sizeof(bool) * 1);
	sockets.fds[0].fd = server_socket;
	sockets.fds[0].events = POLLIN;
	sockets.name[0] = strdup("SERVER");
	return sockets;
}

void push_back(poll_collector *sockets, client_socket const *tmp)
{
	char *address = inet_ntoa(tmp->socket_name.sin_addr);
	char *connected = "CONNECTED\n";

	sockets->fds_n++;

	sockets->name = realloc(sockets->name, sizeof(char *) * sockets->fds_n + 1);
	sockets->name[sockets->fds_n - 1] = malloc(sizeof(char) * (strlen(address) + 1));
	strcpy(sockets->name[sockets->fds_n - 1], address);

	sockets->fds = realloc(sockets->fds, sizeof(struct pollfd) * sockets->fds_n);
	sockets->fds[sockets->fds_n - 1].fd = tmp->fd;
	sockets->fds[sockets->fds_n - 1].events = POLLIN;

	sockets->first_data = realloc(sockets->first_data, sizeof(bool) * (sockets->fds_n));
	sockets->first_data[sockets->fds_n - 1] = true;

	// TODO: in client.c, add the reception of this message below ("CONNECTED")
	write(sockets->fds[sockets->fds_n - 1].fd, connected, strlen(connected));
}

void socket_pop(poll_collector *sockets, int index)
{
	shutdown(sockets->fds[index].fd, 2);
	close(sockets->fds[index].fd);
	free(sockets->name[index]);
	if ((unsigned int) index < (sockets->fds_n - 1)) { // don't count the server's socket
		for (unsigned int i = index; i < (sockets->fds_n - 1); i++) {
			sockets->fds[i] = sockets->fds[i + 1];
			sockets->name[i] = sockets->name[i + 1];
		}
	}
	sockets->fds_n--;
}

char *read_text(poll_collector *sockets, int index)
{
	char buff[256] = {'\0'};
	int ret;

	ret = recv(sockets->fds[index].fd, buff, sizeof(buff), MSG_DONTWAIT);
	if (sockets->first_data[index]) {
		sockets->first_data[index] = false;
		free(sockets->name[index]);
		sockets->name[index] = strndup(buff, strlen(buff) - 1);
		printf("%s CONNECTED\n", sockets->name[index]);
		return NULL;
	}
	if (ret == 0) {
		printf("%s DISCONNECTED\n", sockets->name[index]);
		socket_pop(sockets, index);
		return NULL;
	}
	char *cpy_buff = strdup(buff);
	return cpy_buff;
}

void find_socket(poll_collector *sockets)
{
	char *buff = NULL;
	unsigned int i = 1;
	client_socket tmp = {0};
	
	tmp.addr_len = sizeof(struct sockaddr_in);
	if (sockets->fds[0].revents & POLLIN) {
		tmp.fd = accept(sockets->fds[0].fd, (struct sockaddr *) &tmp.socket_name, &tmp.addr_len);
		push_back(sockets, &tmp);
		printf("NEW CONNECTION\n");
		return;
	}
	for (; i < sockets->fds_n; i++) {
		if (sockets->fds[i].revents & POLLIN) {
			printf("%sGOT DATA FROM %s%s\n", BOLD, sockets->name[i], DEFAULT);
			buff = read_text(sockets, i);
			if (buff != NULL) {
				printf("%s SAYS %s", sockets->name[i], buff);
				break;
			} else
				return;
		}
	}
	for (unsigned int j = 1; j < sockets->fds_n; j++) {
		if (j != i) {
			dprintf(sockets->fds[j].fd, "%s:%s", sockets->name[i], buff);
		}
	}
}

void wait_connections(int server_socket)
{
	poll_collector sockets = create_poll(server_socket);
	int ret;

	while (1) {
		ret = poll(sockets.fds, sockets.fds_n, 5);
		if (ret == -1)
			exit(1);
		if (ret > 0)
			find_socket(&sockets);
	}
}

int main(void)
{
	int server_socket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_socket_name;
	int optval = 1;
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

	// TODO: change fork with threads
	pid = fork();
	if (pid == 0) {
		wait_connections(server_socket);
	} else {
		reading_input();
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		shutdown(server_socket, 2); // stopping server_socket
		close(server_socket); // closing server_socket
		exit(1);
	}
	return 0;
}

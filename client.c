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
	char buff[256];
	WINDOW *output = newwin((LINES - 3), COLS, 0, 0);

	box(output, 0, 0);
	wmove(output, 1, 1);
	wrefresh(output);
	fds[0].fd = args->fd;
	fds[0].events = POLLIN;
	fds[1].fd = args->pipeCP[0];
	fds[1].events = POLLIN;
	while (1) {
		ret = poll(fds, 2, 1 * 1000);
		if (ret == 0)
			continue;
		else if (ret > 0) {
			if (fds[0].revents & POLLIN) {
				read(fds[0].fd, buff, sizeof(buff));
				if (strcmp(buff, "CONNECTED\n") == 0)
					dprintf(fds[0].fd, "%s\n", args->nickname);
				else {
					box(output, 1, 1);
					wmove(output, 1, 1);
					wprintw(output, "%s", buff);
					wrefresh(output);
					wclear(output);
				}
			}
			if (fds[1].revents & POLLIN) {
				read(fds[1].fd, buff, sizeof(buff));
				if (strcmp(buff, "\0") == 0)
					return NULL;
				else
					dprintf(fds[0].fd, "%s", buff);

			}
		}
		refresh();
	}
	return NULL;
}

void read_streams(int socket, char const *nickname)
{
	int pipeCP[2];
	char input[256];
	pthread_t tid;
	struct tid_arg args;
	WINDOW *input_w = newwin(3, COLS, (LINES - 3), 0);
	
	pipe(pipeCP);
	// thread
	args.fd = socket;
	args.pipeCP = malloc(sizeof(int) * 2);
	args.pipeCP[0] = pipeCP[0];
	args.pipeCP[1] = pipeCP[1];
	args.nickname = nickname;
	pthread_create(&tid, NULL, poll_events, &args);
	// no thread
	box(input_w, 0, 0);
	wmove(input_w, 1, 1);
	wrefresh(input_w);
	while (input != NULL) {
		box(input_w, 0, 0);
		wmove(input_w, 1, 1);
		wrefresh(input_w);
		wgetnstr(input_w, input, COLS - 2);
		dprintf(pipeCP[1], "%s\n", input);
		wclear(input_w);
	}
	write(pipeCP[1], "\0", 1);
	pthread_join(tid, NULL);
}

int main(int ac, char **av)
{
	int client_socket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client_socket_name;
	// CHILD -> PARENT
	int pipeCP[2];

	initscr();
	keypad(stdscr, TRUE);
	raw();
	pipe(pipeCP);
	if (ac == 1) {
		printf("Usage:\n\t./client [nickname]\n");
		return 2;
	}
	memset((char *) &client_socket_name, 0, sizeof(struct sockaddr_in));
	client_socket_name.sin_family = AF_INET;
	client_socket_name.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_HOST, &client_socket_name.sin_addr);
	connect(client_socket, (struct sockaddr *) &client_socket_name,
		sizeof(struct sockaddr_in));
	read_streams(client_socket, av[1]);
	endwin();
	shutdown(client_socket, 2);
	close(client_socket);
}

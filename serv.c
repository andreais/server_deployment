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

#define LOCAL_PORT 10000
#define LOCAL_HOST "127.0.0.1"
#define GREEN		"\e[32m"
#define DEFAULT		"\e[0m"
#define BOLD		"\e[1m"

void reading_input(void)
{
	char input[256];

	while (fgets(input, sizeof(input), stdin)) {
		if (strcmp(input, "!q\n") == 0) {
			return;
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

	if (pid == 0 && closing != true) {
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

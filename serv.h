#ifndef SERV_H_
#define SERV_H_

#pragma once

#include <sys/socket.h>
#include <stdbool.h>

#define LOCAL_PORT	10000
#define LOCAL_HOST	"127.0.0.1"
#define GREEN		"\e[32m"
#define DEFAULT		"\e[0m"
#define BOLD		"\e[1m"

typedef struct {
	struct pollfd *fds;
	unsigned int fds_n;
	char **name;
	bool *first_data;
} poll_collector;

typedef struct {
	int fd;
	struct sockaddr_in socket_name;
	unsigned int addr_len;
} client_socket;

#endif /* SERV_H_ */

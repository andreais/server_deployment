#pragma once

#define FAILED_CLIENT_SOCKET 1
#define FAILED_PIPE 2
#define NE_ARGS 3
#define FAILED_CONNECT 4
#define FAILED_THREAD_CREATION 5

#define FAILED_ALLOC 17

struct tid_arg {
	int fd;
	int *pipeCP;
	char const *nickname;
	int *stop_server;
};


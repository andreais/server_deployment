#ifndef CLIENT_H_
#define CLIENT_H_

#pragma once

struct tid_arg {
	int fd;
	int *pipeCP;
	char const *nickname;
};

#endif /* CLIENT_H_ */

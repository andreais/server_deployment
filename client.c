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

char *get_next_line(int fd);

int main(int ac, char **av)
{
	int client_socket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client_socket_name;

	if (ac == 1) {
		printf("%sCareful%s ! You're entering free mode.\n", BOLD, DEFAULT);
	}
	memset((char *) &client_socket_name, 0, sizeof(struct sockaddr_in));
	client_socket_name.sin_family = AF_INET;
	client_socket_name.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_HOST, &client_socket_name.sin_addr);

	connect(client_socket, (struct sockaddr *) &client_socket_name,
		sizeof(struct sockaddr_in));
	if (ac > 1) {
		write(client_socket, av[1], strlen(av[1]));
		write(client_socket, "\n", 1);
	} else {
		char *buff;
		while ((buff = get_next_line(0)) != NULL) {
			write(client_socket, buff, strlen(buff));
			write(client_socket, "\n", 1);
		}
	}
	shutdown(client_socket, 2);
	close(client_socket);
}

int check_buffer(char *buff)
{
 for (int i = 0; buff[i]; i++)
  if (buff[i] == '\n')
   return (0);
 return (1);
}

int my_strlen2(char *str, char c)
{
 int length = 0;

 if (str == NULL)
  return (-1);
 for (; str[length] != '\0' || str[length] == c; length++)
  if (str[length] == c)
   return (length);
 return (-1);
}

char *my_strjoin(char *begin, char *end, int i)
{
 char *tmp;

 if (!(tmp = malloc (sizeof(char) * (i + 1))))
  return (NULL);
 tmp[i] = '\0';
 for (int k = 0; k < i - 1; k++)
  tmp[k] = begin[k];
 tmp[i - 1] = end[0];
 free(begin);
 return (tmp);
}

char *get_next_line(int fd)
{
 static char buffer[1];
 int i = 0;
 char *ret;
 int test = 0;

 if (!(ret = malloc (sizeof(char) * 1)))
  return (NULL);
 ret[0] = '\0';
 while ((test = read(fd, buffer, 1)) != 0) {
  i++;
  if (!check_buffer(buffer)) {
   buffer[0] = '\0';
   return (ret);
  }
  ret = my_strjoin(ret, buffer, i);
 }
 if (!ret[0]) {
  free(ret);
  return (NULL);
 } else
  return (ret);
}

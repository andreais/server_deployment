##
## makefile fed in the abyss
## rudreais
##
##

CC		=	gcc

CFLAGS		+=	-I $(DIRINC)	\
			-Wall -Wextra -W	\
			-g -lncurses

DIRSRC		+=	.	

DIRINC		+=	include/

SRC		+=	$(DIRSRC)client.c	\
			$(DIRSRC)serv.c

OBJ		=	$(SRC:.c=.o)

EXEC		=	project

all:		$(EXEC)

$(EXEC):	$(OBJ)
		$(CC) -o client client.o $(CFLAGS)
		$(CC) -o serv serv.o
clean:
		rm -f *.o

fclean:		clean
		rm -rf client serv

re:		fclean all

.PHONY:		all clean fclean re

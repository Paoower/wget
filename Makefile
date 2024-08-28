NAME=wget

SRC_DIR=./srcs/
INC_DIR=./includes/

SRC=main.c
SRCS=$(addprefix ${SRC_DIR}, ${SRC})
OBJS=${SRCS:.c=.o}

CC=gcc
FLAGS=-Wall -Wextra -Werror
RM=rm -f

.c.o:
	${CC} ${FLAGS} -I${INC_DIR} -c $< -o ${<:.c=.o}

${NAME}: ${OBJS}
	${CC} ${FLAGS} -o ${NAME} ${OBJS}

all: ${NAME}

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re

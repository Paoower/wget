NAME=wget

INCLUDES_DIR=./includes/
SRCS_DIR=./srcs/
TOOLS_DIR=$(SRCS_DIR)tools/

SRCS_FILE_NAMES=main.c download_file.c
TOOLS_FILE_NAMES=regex.c string.c

SRC_FILES=$(addprefix ${SRCS_DIR}, ${SRCS_FILE_NAMES})
TOOLS_FILES=$(addprefix ${TOOLS_DIR}, ${TOOLS_FILE_NAMES})

FILES=$(SRC_FILES) $(TOOLS_FILES)
OBJS=${FILES:.c=.o}

CC=gcc
FLAGS=-Wall -Wextra -Werror
RM=rm -f

.c.o:
	${CC} ${FLAGS} -I${INCLUDES_DIR} -c $< -o $@

${NAME}: ${OBJS}
	${CC} ${FLAGS} -o ${NAME} ${OBJS}

all: ${NAME}

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re

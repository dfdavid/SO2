CC=gcc
CFLAGS= -Werror -Wall -pedantic

all: programa_servidor

programa_servidor: main.c
	cppcheck --enable=all --inconclusive --std=posix main.c
	$(CC) $(CFLAGS) -o programa_servidor main.c 

clean: 
	rm programa_servidor 

CC=gcc
CFLAGS= -Werror -Wall -pedantic

all: tp1_server

tp1_server: main.c
	cppcheck --enable=all --inconclusive --std=posix main.c
	$(CC) $(CFLAGS) -o tp1_server main.c 

clean: 
	rm tp1_server 

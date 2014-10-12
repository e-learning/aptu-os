CFLAGS=-Werror -Wall
CC=gcc

all: shell

shell: main.o 
	$(CC) main.o -o shell

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c

.PHONY: clean

clean:
	rm -rf *o 
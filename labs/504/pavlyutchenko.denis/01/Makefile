CFLAGS=-Werror -Wall
CC=g++

all: shell

shell: main.o 
	$(CC) main.o -o shell

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp 

.PHONY: clean

clean:
	rm -rf *o hello
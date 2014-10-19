CFLAGS=-Werror -Wall -g -O0
CC=g++ -std=c++11

all: memory

memory: main.o
	$(CC) main.o -o memory

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp 

.PHONY: clean

clean:
	rm -rf *o memory
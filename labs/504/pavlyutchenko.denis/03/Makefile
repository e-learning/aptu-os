CFLAGS=-Werror -Wall -g -O0
CC=g++ -std=c++11

all: allocator

allocator: main.o
	$(CC) main.o -o allocator

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp 

.PHONY: clean

clean:
	rm -rf *o allocator
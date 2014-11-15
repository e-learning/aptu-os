CFLAGS=-Werror -Wall -g -O0
CC=g++

all: traceroute

traceroute: main.o
	$(CC) main.o -o traceroute

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp

.PHONY: clean

clean:
	rm -rf *o traceroute
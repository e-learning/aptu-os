CFLAGS=-Werror -Wall -g -O0
CC=g++

all: scheduler

scheduler: main.o Task.o
	$(CC) main.o Task.o -o scheduler

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp 

Task.o: Task.cpp
	$(CC) $(CFLAGS) -c Task.cpp 

.PHONY: clean

clean:
	rm -rf *o scheduler
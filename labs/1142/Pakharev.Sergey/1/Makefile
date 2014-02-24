CC=g++
CFLAGS=-c -Wall

all: shell

shell: main.o
	$(CC) main.o -o shell
	rm *.o    
main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp


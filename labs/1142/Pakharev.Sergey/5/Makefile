CC=g++
CFLAGS=-c -Wall

all: pop3

pop3: main.o
	$(CC) main.o -o pop3
	rm *.o    
main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp


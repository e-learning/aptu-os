CFLAGS= -c -Wall -O2 -p -pg 

all: allocator

allocator:  allocator.o  main.o 
	g++  allocator.o  main.o -p -pg -o allocator

allocator.o: allocator.cpp allocator.h 
	g++ $(CFLAGS) allocator.cpp

main.o: main.cpp
	g++ $(CFLAGS)  main.cpp

clean:
	rm -rf *.o allocator

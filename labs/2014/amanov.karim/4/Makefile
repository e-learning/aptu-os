CFLAGS= -c -Wall   

all:  allocator

allocator: main.o allocator.o   
	g++ main.o allocator.o -o allocator

main.o: main.cpp
	g++ $(CFLAGS)  main.cpp

allocator.o: allocator.cpp
	g++ $(CFLAGS) allocator.cpp

clean:
	rm -rf *.o allocator 


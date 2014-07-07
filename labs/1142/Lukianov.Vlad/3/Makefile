allocator: main.o Allocator.o
	g++ main.o Allocator.o -o allocator

main.o: main.cpp
	g++ -c main.cpp

Allocator.o: Allocator.h Allocator.cpp
	g++ -c Allocator.cpp

clean:
	rm -rf *.o allocator
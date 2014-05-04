allocator: main.o allocator.o
	g++ main.o allocator.o -o allocator

main.o: main.cpp
	g++ -c main.cpp

allocator.o: allocator.h allocator.cpp
	g++ -c allocator.cpp

clean:
	rm -rf *.o allocator


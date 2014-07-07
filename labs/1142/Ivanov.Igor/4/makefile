default:  clean allocator
main.o: MemoryAlloc.cpp 
	g++ -c -pedantic -std=c++11  MemoryAlloc.cpp  
allocator: main.o 
	g++ MemoryAlloc.o -o allocator
clean:
	rm -rf *.o allocator


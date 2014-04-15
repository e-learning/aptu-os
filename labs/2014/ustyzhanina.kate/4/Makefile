default:  clean allocator
main.o: main.cpp 
	g++ -c -pedantic -std=c++11  main.cpp  
allocator: main.o 
	g++ main.o -o allocator
clean:
	rm -rf *.o allocator


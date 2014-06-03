default: allocator

allocator: main.o
	g++ main.o -o allocator
	
main.o: main.cpp
	g++ -c main.cpp -o main.o 


clean:
	rm allocator
	rm *.o
	
.PHONY: clean all

default: memory

memory: main.o
	g++ -std=c++11 main.o -o memory
	
main.o: main.cpp
	g++ -std=c++11 -c main.cpp -o main.o 


clean:
	rm memory
	rm *.o
	
.PHONY: clean all

default: shell

shell: main.o
	g++ -O2 -Wall -std=c++11 main.o -o shell
	
main.o: main.cpp
	g++ -O2 -Wall -std=c++11 -c main.cpp -o main.o 
	
clean:
	rm -f *.o
	rm -f shell
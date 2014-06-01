default: shell clean

shell: main.o
	g++ main.o -o shell
	
main.o: main.cpp
	g++ -c main.cpp -o main.o 
	
clean:
	rm -f *.o

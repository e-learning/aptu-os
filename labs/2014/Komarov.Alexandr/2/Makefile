default: scheduler clean

scheduler: main.o
	g++ main.o -o scheduler
	
main.o: main.cpp
	g++ -c main.cpp -o main.o 
	
clean:
	rm -f *.o


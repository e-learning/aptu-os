default: scheduler

scheduler: main.o
	g++ main.o -o scheduler
	
main.o: main.cpp
	g++ -c main.cpp -o main.o 


clean:
	rm scheduler
	rm *.o

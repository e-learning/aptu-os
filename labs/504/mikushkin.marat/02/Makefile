scheduler: main.o process.o scheduler.o
	g++ -o scheduler main.o process.o scheduler.o; rm *.o
process.o: process.cpp process.h
	g++ -Wall -c process.cpp
scheduler.o: scheduler.cpp scheduler.h
	g++ -Wall -c scheduler.cpp
main.o: main.cpp process.h scheduler.h
	g++ -Wall -c main.cpp

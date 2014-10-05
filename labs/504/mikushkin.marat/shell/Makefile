shell: main.o functions.o
	g++ -o shell main.o functions.o; rm *.o
functions.o: functions.cpp general.h
	g++ -Wall -c functions.cpp
main.o: main.cpp general.h
	g++ -Wall -c main.cpp

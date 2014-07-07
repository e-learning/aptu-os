v2: main2.o libps.so
	g++ main2.o -L. -ldl

main2.o: main2.cpp
	gcc -o main2.o -c main2.cpp

v1: main.o libps.so
	g++ main.o -L. -lps -Wl,-rpath,.

main.o: main.cpp
	gcc -o main.o -c main.cpp

ps.o: ps.h ps.cpp
	gcc -o ps.o -c ps.cpp

libps.so: ps.o
	gcc -shared -o libps.so ps.o

clear:
	rm *.o
run: ver2
	./a.out

ver1: main.o comminterp.o libps.so
	g++ main.o comminterp.o -L. -lps -Wl,-rpath,.

ver2: MAIN.o  libps.so
	g++ MAIN.o -ldl

main.o: main.cpp
	gcc -o main.o -c main.cpp

ps.o: ps.h ps.cpp
	gcc -o ps.o -c ps.cpp

libps.so: ps.o
	gcc -shared -o libps.so ps.o

clear:
	rm *.o

clearAll: clear
	rm a.out
	rm *.so

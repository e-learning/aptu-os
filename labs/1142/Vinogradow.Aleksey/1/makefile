run: shell
	./shell

shell: main.o comminterp.o libps.so
	g++ -o shell main.o comminterp.o -ldl

main.o: main.cpp 
	gcc -o main.o -c main.cpp

comminterp.o: comminterp.h comminterp.cpp
	gcc -o comminterp.o -c comminterp.cpp

ps.o: ps.h ps.cpp
	gcc -o ps.o -c ps.cpp

libps.so: ps.o
	gcc -shared -o libps.so ps.o

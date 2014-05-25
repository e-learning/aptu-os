run: shell
	./a.out

shell: main.o comminterp.o
	g++ main.o comminterp.o

main.o: main.cpp 
	gcc -o main.o -c main.cpp

comminterp.o: comminterp.h comminterp.cpp
	gcc -o comminterp.o -c comminterp.cpp

ps.o: ps.h ps.cpp
	gcc -o ps.o -c ps.cpp

clear:
	rm *.o

clearAll: clear
	rm a.out

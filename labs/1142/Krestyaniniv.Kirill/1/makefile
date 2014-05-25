run: shell
	./a.out

shell: main.o 
	g++ main.o

main.o: main.cpp
	gcc -o main.o -c main.cpp

clear:
	rm *.o

clearAll: clear
	rm a.out

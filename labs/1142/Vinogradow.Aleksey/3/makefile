run: exe
	./a.out

exe: main.o a.o b.o
	g++ main.o r_a.o r_b.o

main.o: main.cpp
	gcc -o main.o -c main.cpp

a.o: r_a.cpp
	gcc -o r_a.o -c r_a.cpp

b.o: r_b.cpp
	gcc -o r_b.o -c r_b.cpp

clear:
	rm *.o

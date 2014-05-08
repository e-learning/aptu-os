exe: obj lib
	g++ main.o -L. -lps -o shell -Wl,-rpath,.
obj: main.cpp
	g++ -c main.cpp -o main.o
lib: 
	g++ -shared ps.cpp -o libps.so
clear:
	rm *.o


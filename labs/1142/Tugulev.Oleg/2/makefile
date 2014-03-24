sh: main pslib
	g++ main.o -L. -lps -o sh -Wl,-rpath,.
main: main.cpp
	g++ -c main.cpp -o main.o
pslib:
	g++ -shared ps.cpp -o libps.so
clean:
	rm -rf *.so
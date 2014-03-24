sh1: main pslib sh2
	g++ main.o -L. -lps -o shell1 -Wl,-rpath,.
sh2: main pslib
	g++ mainldl.o -L. -ldl -o shell2 -Wl,-rpath,.
main: main.cpp
	g++ -c main.cpp -o main.o
pslib:
	g++ -shared ps.cpp -o libps.so
clean:
	rm -rf *.so
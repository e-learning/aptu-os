all: shell

shell: ps
	g++ main.cpp -o shell libps.so
ps: 
	g++ -c -std=c++11 -fPIC -o ps.o  ps.cpp && g++ -shared -o libps.so ps.o

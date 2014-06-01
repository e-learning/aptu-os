CXX = g++
CFLAGS= -c -Wall -O2 -p -pg -std=c++0x 
CXXFLAGS = -O2 -Wall -std=c++0x 

all: memory

memory: main.o
	g++  $(CXXFLAGS) main.o -o memory
main.o: main.cpp
	g++ $(CFLAGS) main.cpp

clean:
	rm -rf *.o memory
	


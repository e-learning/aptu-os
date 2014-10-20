CPP_FLAGS=-O2 -Wall -Werror -std=c++11

all: memory

lib.o: lib.cpp lib.h
	g++ $(CPP_FLAGS) -c lib.cpp -o lib.o

memory.o: memory.cpp lib.h
	g++ $(CPP_FLAGS) -c memory.cpp -o memory.o

memory:	memory.o lib.o
	g++ $(CPP_FLAGS) memory.o lib.o -o memory

clean:
	rm -f memory.o memory

CPP_FLAGS=-O2 -Wall -Werror
all: shell
sh.o: sh.cpp sh.h
	g++ $(CPP_FLAGS) -c sh.cpp -o sh.o
main.o: main.cpp sh.h
	g++ $(CPP_FLAGS) -c main.cpp -o main.o
shell: sh.o main.o
	g++ $(CPP_FLAGS) sh.o main.o -o shell
clean:
	rm -rf shell

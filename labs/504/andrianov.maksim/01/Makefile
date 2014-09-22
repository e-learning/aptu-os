CPP_FLAGS=-O2 -Wall -Werror
# -v
all: main
ps.o: ps.cpp ps.h
	g++ $(CPP_FLAGS) -c ps.cpp -o ps.o
main.o: main.cpp ps.h
	g++ $(CPP_FLAGS) -c main.cpp -o main.o
main: ps.o main.o
	g++ $(CPP_FLAGS) ps.o main.o -o main
clean:
	rm -rf main

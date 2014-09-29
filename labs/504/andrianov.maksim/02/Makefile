CPP_FLAGS=-O2 -Wall -Werror
# -v
all: scheduler
main.o: main.cpp
	g++ $(CPP_FLAGS) -c main.cpp -o main.o
scheduler: main.o
	g++ $(CPP_FLAGS) main.o -o scheduler
clean:
	rm -rf scheduler

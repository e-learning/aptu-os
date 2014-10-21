FLAGS=-O2 -Wall -Werror
CC=g++
all: memory

memory: ./src/main.cpp
	mkdir -p ./bin
	$(CC) $(FLAGS) -c ./src/main.cpp -o ./bin/main.o
	$(CC) $(FLAGS) ./bin/main.o -o memory

.PHONY: clean
clean:
	rm -f memory ./bin/*

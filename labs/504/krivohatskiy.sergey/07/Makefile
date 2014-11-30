FLAGS=-O2 -Wall -Werror
CC=g++
all:  traceroute

traceroute: ./src/main.cpp ./src/traceroot.cpp ./src/traceroot.h
	mkdir -p ./bin
	$(CC) $(FLAGS) -c ./src/main.cpp -o ./bin/main.o
	$(CC) $(FLAGS) -c ./src/traceroot.cpp -o ./bin/traceroot.o
	$(CC) $(FLAGS) ./bin/main.o ./bin/traceroot.o -o traceroute

.PHONY: clean
clean:
	rm -f traceroute ./bin/*

FLAGS=-O2 -Wall -Werror
CC=g++
all: sieve

sieve: ./sieve_multith.cpp ./sieve_unith.cpp
	$(CC) -c sieve_multith.cpp -pthread -std=c++11
	$(CC) sieve_multith.o -o sieve_multith -pthread -std=c++11
	$(CC) -c sieve_unith.cpp -pthread -std=c++11
	$(CC) sieve_unith.o -o sieve_unith -pthread -std=c++11

.PHONY: clean
clean:
	rm -f memory ./sieve_multith.o sieve_unith.o sieve_unith sieve_multith

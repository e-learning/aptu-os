CPP_Flags=-Werror -Wall -std=c++0x
CPP=g++

all: sieve_multith sieve_unith

sieve_multith: sieve_multith.o
	$(CPP) $(CPP_Flags) -pthread sieve_multith.o -o sieve_multith

sieve_multith.o: sieve_multith.cpp
	$(CPP) $(CPP_Flags) -c sieve_multith.cpp

sieve_unith: sieve_unith.o
	$(CPP) $(CPP_Flags) sieve_unith.o -o sieve_unith

sieve_unith.o: sieve_unith.cpp
	$(CPP) $(CPP_Flags) -c sieve_unith.cpp

.PHONY: clean

clean:
	rm -f sieve_unith.o sieve_unith sieve_multith.o sieve_multith
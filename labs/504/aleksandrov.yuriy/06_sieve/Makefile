FLAGS=-Wall -Werror -O2 -std=c++0x
AUXILIARY=texceptions.cpp get_params.cpp
AUXILIARY_HEADERS=texceptions.hpp get_params.hpp
all: sieve_unith sieve_multith 
sieve_unith: $(AUXILIARY_HEADERS)
	g++ $(FLAGS) sieve_unith.cpp $(AUXILIARY) -o sieve_unith
sieve_multith: $(AUXILIARY_HEADERS)
	g++ $(FLAGS) -pthread sieve_multith.cpp $(AUXILIARY) -o sieve_multith
.PHONY: clean

clean:
	rm -f *.o *.time sieve_unith sieve_multith

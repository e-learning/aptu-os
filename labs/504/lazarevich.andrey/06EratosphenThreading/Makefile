all:
	g++ -Wall -Werror -std=c++11 sieve_unith.cpp -o sieve_unith
	g++ -Wl,--no-as-needed -Wall -Werror -pthread -std=c++11 sieve_multith.cpp -o sieve_multith
.PHONY: clean

clean:
	rm -f sieve_unith sieve_multith

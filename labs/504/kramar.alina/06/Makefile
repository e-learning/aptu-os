CPP_FLAGS_M=-Wl,--no-as-needed -Wall -Werror -pthread -std=c++11
CPP_FLAGS_U=-Wall -Werror -std=c++11

all:
	g++ $(CPP_FLAGS_M) sieve_multith.cpp -o sieve_multith
	g++ $(CPP_FLAGS_U) sieve_unith.cpp -o sieve_unith

clean:
	rm -f sieve_unith sieve_multith

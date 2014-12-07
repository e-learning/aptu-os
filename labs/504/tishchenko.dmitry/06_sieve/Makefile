all: sieve_unith sieve_multith

sieve_unith:
	g++ sieve_unith.cpp -o sieve_unith

sieve_multith:
	g++ -std=c++11 -pthread sieve_multith.cpp -o sieve_multith

clean:
	rm sieve_unith sieve_multith

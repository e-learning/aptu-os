OUT1=./sieve_unith
SRC1=unith.cpp bit_array.cpp
OUT2=./sieve_multith
SRC2=multith.cpp bit_array.cpp
build: clean
	g++ -std=c++11 -O2 -Werror -Wall -o $(OUT1) $(SRC1)
	g++ -std=c++11 -pthread -O2 -Werror -Wall -o $(OUT2) $(SRC2)
clean:
	rm -f $(OUT)

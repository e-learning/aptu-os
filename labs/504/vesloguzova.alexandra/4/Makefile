CPP_Flags = -Werror -Wall  -std=c++11
memory: main.o
	g++ ${CPP_Flags} main.o  -o memory
main.o: main.cpp
	g++ $(CPP_Flags) -c main.cpp
clean:
	rm -f allocator main.o
.PHONY: clean
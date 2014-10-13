all: allocator


allocator: main.cpp common.h
	g++ -Wall -Werror -std=c++0x main.cpp common.h -o allocator

.PHONY: clean

clean:
	rm -f allocator

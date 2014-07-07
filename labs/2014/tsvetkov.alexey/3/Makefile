CC := g++
CFLAGS := -Wall -std=c++11

COMMON := file_system.hpp structs.hpp
COMMANDS := init format import export ls copy move rm mkdir

.PHONY := clean

all: file_system.o $(COMMANDS)

file_system.o: file_system.cpp $(COMMON)
	$(CC) -c file_system.cpp -o file_system.o $(CFLAGS)

$(COMMANDS): file_system.o $(COMMON)
	$(CC) $@.cpp file_system.o $(CFLAGS) -o $@

clean:
	rm -f $(COMMANDS)
	rm -f *.o


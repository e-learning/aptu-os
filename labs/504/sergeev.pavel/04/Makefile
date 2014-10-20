CC=gcc
FLAGS=-Wall -Werror -g

all: memory

main.o: main.c memory.h
	$(CC) $(FLAGS) -c main.c

memory.o: memory.c memory.h
	$(CC) $(FLAGS) -c memory.c

memory: main.o memory.o
	$(CC) $(FLAGS) main.o memory.o -o memory

.PHONY: clean

clean:
	rm -f *.o memory
CC=gcc
FLAGS=-Wall -Werror -g

all: allocator

main.o: main.c cmd_interface.h
	$(CC) $(FLAGS) -c main.c

cmd_interface.o: cmd_interface.c cmd_interface.h allocator.h
	$(CC) $(FLAGS) -c cmd_interface.c

allocator.o: allocator.c allocator.h
	$(CC) $(FLAGS) -c allocator.c

allocator: main.o cmd_interface.o allocator.o
	$(CC) $(FLAGS) main.o cmd_interface.o allocator.o -o allocator

.PHONY: clean

clean:
	rm -f *.o allocator
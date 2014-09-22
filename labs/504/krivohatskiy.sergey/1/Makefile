FLAGS=-O2 -Wall -Werror

all: shell

syscalls.o: syscalls.h syscalls.c
	$(CC) $(FLAGS) -c syscalls.c

command_executing.o: command_executing.c command_executing.h
	$(CC) $(FLAGS) -c command_executing.c

main.o: command_executing.o
	$(CC) $(FLAGS) -c main.c

shell: command_executing.o syscalls.o main.o
	$(CC) $(FLAGS) command_executing.o syscalls.o main.o -o shell

.PHONY: clean
clean:
	rm -f shell command_executing.o syscalls.o main.o

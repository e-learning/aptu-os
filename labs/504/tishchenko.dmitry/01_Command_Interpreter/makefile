C-FLAGS = -Wall -Werror
all: shell
shell.o: shell.c
	gcc $(C-FLAGS) -c shell.c -o shell.o
shell: shell.o
	gcc $(C-FLAGS) shell.o -o shell
clean:
	rm -rf shell

CPP_Flags = -Werror -Wall
shell: main.o
	gcc ${CPP_Flags} main.o  -o shell
main.o: main.c
	gcc $(CPP_Flags) -c main.c
clean:
	rm -f allocator main.o
.PHONY: clean
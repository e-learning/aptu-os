CPP_Flags = -Werror -Wall
allocator: main.o 
	gcc ${CPP_Flags} main.o  -o allocator
main.o: main.c
	gcc $(CPP_Flags) -c main.c
clean:
	rm -f allocator main.o
.PHONY: clean
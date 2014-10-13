C-FLAGS = -Wall -Werror
all: allocator
main.o: main.c
	gcc $(C-FLAGS) -c main.c -o main.o
heap_functions.o:  heap_functions.c
	gcc $(C-FLAGS) -c heap_functions.c -o heap_functions.o
requests.o: requests.c
	gcc $(C-FLAGS) -c requests.c -o requests.o
allocator: main.o heap_functions.o requests.o
	gcc $(C-FLAGS) main.o heap_functions.o requests.o -o allocator
clean:
	rm -rf allocator

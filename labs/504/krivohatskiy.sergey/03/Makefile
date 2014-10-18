FLAGS=-O2 -Wall -Werror

all: allocator

./bin/allocator.o: ./src/allocator.c ./src/allocator.h
	mkdir -p ./bin
	$(CC) $(FLAGS) -c ./src/allocator.c -o ./bin/allocator.o
./bin/main.o: ./src/allocator.h ./src/main.c 
	mkdir -p ./bin
	$(CC) $(FLAGS) -c ./src/main.c -o ./bin/main.o
allocator: ./bin/allocator.o ./bin/main.o
	$(CC) $(FLAGS) ./bin/allocator.o ./bin/main.o -o allocator

.PHONY: clean
clean:
	rm -f allocator ./bin/*

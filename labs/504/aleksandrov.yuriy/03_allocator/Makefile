FLAGS = -O2 -Wall -Werror -std=c99
CC = gcc
OUT = allocator
DEPS = auxiliary.c commands.c

all:
	$(CC) $(FLAGS) $(DEPS) $(OUT).c -o $(OUT)
clean:
	rm -f $(OUT)


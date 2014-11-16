FLAGS=-O2 -Wall -Werror 
CC = gcc
OUT = shell.bin
DEPS = auxiliary.c commands.c shell.c

all: libps.so
	chmod +x ./shell
	$(CC) $(FLAGS) $(DEPS) -L. -lps -o $(OUT)
libps.so:
	$(CC) $(FLAGS) -shared -fPIC ps.c -o libps.so
debug: ps.o
	$(CC) $(FLAGS) -ggdb  $(DEPS) -o $(OUT)
.PHONY: clean

clean:
	rm -f $(OUT) libps.so

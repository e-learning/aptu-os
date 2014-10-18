FLAGS=-O2 -Wall -Werror 
CC = gcc
OUT = shell
DEPS = auxiliary.c commands.c shell.c


all: 
	$(CC) $(FLAGS)  $(DEPS) -o $(OUT)
.PHONY: clean

clean:
	rm -f $(OUT)

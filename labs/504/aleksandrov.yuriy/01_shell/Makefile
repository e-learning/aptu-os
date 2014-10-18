FLAGS=-O2 -Wall -Werror 
CC = gcc
OUT = shell
DEPS = auxiliary.c commands.c shell.c


all: 
	$(CC) $(FLAGS)  $(DEPS) -o $(OUT)
valgrind:
	$(CC) $(FLAGS)  -g -lm $(DEPS) -o $(OUT)
	valgrind --tool=memcheck --leak-check=yes ./$(OUT)
.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o $(OUT)

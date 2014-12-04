CC=gcc
FLAGS=-Wall -Werror
OBJECTS=main.o sh.o commands.o utils.o

all: shell

main.o: main.c sh.h 
	$(CC) $(FLAGS) -c main.c

sh.o: sh.c sh.h commands.c
	$(CC) $(FLAGS) -c sh.c

utils.o: utils.c utils.h
	$(CC) $(FLAGS)  -c utils.c 
	
commands.o: commands.c commands.h utils.c
	$(CC) $(FLAGS) -fPIC -c commands.c

libcommands.so: commands.o
	$(CC) -shared -o libcommands.so commands.o

shell: main.o sh.o libcommands.so utils.o
	$(CC) $(FLAGS) main.o sh.o utils.o -L. -lcommands -o shell

.PHONY: clean

clean:
	rm -f shell $(OBJECTS) libcommands.so
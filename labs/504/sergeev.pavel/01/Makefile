CC=gcc
FLAGS=-Wall -Werror
OBJECTS=main.o utils.o shell.o

all: shell

main.o: main.c shell.h utils.h
	$(CC) $(FLAGS) -c main.c

shell.o: shell.c shell.h
	$(CC) $(FLAGS) -c shell.c

utils.o: utils.c utils.h
	$(CC) $(FLAGS) -fPIC -c utils.c

libutils.so: utils.o
	$(CC) -shared -o libutils.so utils.o

shell: main.o shell.o libutils.so
	$(CC) $(FLAGS) main.o shell.o -L. -lutils -o shell

.PHONY: clean

clean:
	rm -f shell libutils.so $(OBJECTS)
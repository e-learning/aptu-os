CC=gcc
FLAGS=-Wall -Werror -g

all: scheduler

main.o: main.c scheduler.h
	$(CC) $(FLAGS) -c main.c

scheduler.o: scheduler.c scheduler.h
	$(CC) $(FLAGS) -c scheduler.c

scheduler: main.o scheduler.o
	$(CC) $(FLAGS) main.o scheduler.o -o scheduler

.PHONY: clean

clean:
	rm -f *.o scheduler
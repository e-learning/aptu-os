FLAGS=-O2 -Wall -Werror

all: scheduler

heap.o: heap.h heap.c
	$(CC) $(FLAGS) -c heap.c

sheduler_emulator.o: sheduler_emulator.h sheduler_emulator.c heap.h
	$(CC) $(FLAGS) -c sheduler_emulator.c

main.o: sheduler_emulator.h main.c 
	$(CC) $(FLAGS) -c main.c

scheduler: heap.o sheduler_emulator.o main.o
	$(CC) $(FLAGS) heap.o sheduler_emulator.o main.o -o scheduler

.PHONY: clean
clean:
	rm -f scheduler heap.o sheduler_emulator.o main.o

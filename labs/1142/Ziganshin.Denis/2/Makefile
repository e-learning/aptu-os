#!/bin/bash
part1: obj1 lib part2
	gcc 2.1.o -L. -lps -o shell1 -Wl,-rpath,.
part2: obj2 lib
	gcc 2.2.o -L. -ldl -o shell2  -Wl,-rpath,.
obj1: 2.1.c
	gcc -c 2.1.c -o 2.1.o
obj2: 2.2.c
	gcc -c 2.2.c -o 2.2.o
lib: 
	gcc -shared ps.c -o libps.so
clear:
	rm *.o


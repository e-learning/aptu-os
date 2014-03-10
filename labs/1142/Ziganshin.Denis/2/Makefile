#!/bin/bash
exe: obj lib
	gcc 1.o -L. -lps -o shell -Wl,-rpath,.
obj: 1.c
	gcc -c 1.c -o 1.o
lib: 
	gcc -shared ps.c -o libps.so
clear:
	rm *.o


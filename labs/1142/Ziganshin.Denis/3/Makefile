#!/bin/bash
part1: obj1 part2
	gcc 3.1.o -o shell1
part2: obj2 
	gcc 3.2.o -L. -pthread -o shell2  -Wl,-rpath,.
obj1: main.cpp
	gcc -c main.cpp -o 3.1.o
obj2: main_pthread.cpp
	gcc -c main_pthread.cpp -o 3.2.o
clear:
	rm *.o


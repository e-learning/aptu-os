
shell: main.o stdafx.o libps.so
	gcc -o main.o stdafx.o -L. -lps -Wl,-rpath,. -ldl

main.o: main.cpp
	gcc -o main.o -c main.cpp

stdafx.o: stdafx.h pslib.h
	gcc -o stdafx.o -c pslib.h

pslib.o: pslib.h pslib.cpp
	gcc -o pslib.o -c pslib.cpp

libps.so: pslib.o
	gcc -shared pslib.o -o libps.so
clear:
	rm *.o

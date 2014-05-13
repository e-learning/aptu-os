
shell: main.o libps.so
	g++ -fPIC -o shell main.o -L. -lps -Wl,-rpath,. -ldl

main.o: main.cpp
	g++ -fPIC -o main.o -c main.cpp

pslib.o: pslib.h pslib.cpp
	g++ -fPIC -o pslib.o -c pslib.cpp

libps.so: pslib.o
	g++ -fPIC -shared pslib.o -o libps.so
clear:
	-rm *.o
	-rm *.so
	-mm shell

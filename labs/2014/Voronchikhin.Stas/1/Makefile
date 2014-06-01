CFLAGS= -c -Wall -O2 -p -pg 

all: main.o
	g++ main.o  -Wall -O2 -p -pg  -o shell

main.o: 
	g++ $(CFLAGS)  main.cpp 

clean:
	rm -rf *.o shell

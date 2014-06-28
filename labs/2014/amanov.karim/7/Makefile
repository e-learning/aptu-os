CFLAGS= -c -Wall -std=c++11  

all: elfreader 

elfreader: main.o    
	g++ main.o -o elfreader

main.o: main.cpp
	g++ $(CFLAGS)  main.cpp

clean:
	rm -rf *.o elfreader 


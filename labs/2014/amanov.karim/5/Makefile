CFLAGS= -c -Wall   

all:  memory

memory: main.o    
	g++ main.o -o memory

main.o: main.cpp
	g++ $(CFLAGS)  main.cpp

clean:
	rm -rf *.o memory 


all: shell

shell: main.o
	g++ main.o

main.o: main.cpp
	g++ -c main.cpp
	
clean:
	rm -rf *.o

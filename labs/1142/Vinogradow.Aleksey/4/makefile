run: exe
	./MemoryAllocator

exe: main.o comminterp.o
	g++ -o MemoryAllocator main.o comminterp.o

main.o: main.cpp 
	g++ -o main.o -c main.cpp

comminterp.o: comminterp.h comminterp.cpp
	g++ -o comminterp.o -c comminterp.cpp

clear: *.o
	rm *.o


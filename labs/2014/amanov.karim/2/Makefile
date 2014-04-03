CFLAGS= -c -std=c++0x  

all: scheduler

scheduler : main.o scheduler.o proc.o processor.o     
	g++  main.o scheduler.o proc.o processor.o -o scheduler 

proc.o: proc.cpp
	g++ $(CFLAGS) proc.cpp

main.o: main.cpp
	g++ $(CFLAGS)  main.cpp

scheduler.o: scheduler.cpp
	g++ $(CFLAGS) scheduler.cpp

processor.o: processor.cpp
	g++ $(CFLAGS) processor.cpp
clean:
	rm -rf *.o scheduler



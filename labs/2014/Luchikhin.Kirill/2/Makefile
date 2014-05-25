default: scheduler

scheduler: main.o Process.o OS.o Scheduler.o
	g++ main.o Process.o OS.o Scheduler.o -o scheduler

main.o: main.cpp
	g++ -c main.cpp

Process.o: src/Process.cpp
	g++ -c src/Process.cpp 

OS.o: src/OS.cpp
	g++ -c src/OS.cpp

Scheduler.o: src/Scheduler.cpp
	g++ -c src/Scheduler.cpp

clean:
	rm -rf *.o scheduler


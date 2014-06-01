default: scheduler

scheduler: main.o
	g++ -O2 -Wall -std=c++11 bin/main.o -o scheduler
	
main.o:
	mkdir -p bin
	g++ -O2 -Wall -std=c++11 -c src/main.cpp -o bin/main.o
	
clean:
	rm -rf bin scheduler
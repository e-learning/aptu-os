CFLAGS= -c -std=c++0x -Wall   

all: shell 

shell: parser.o main.o lexer.o interpreter.o executor.o libpscmd.so   
	g++ -o shell parser.o main.o lexer.o interpreter.o executor.o -L. -lpscmd -Wl,-rpath,.

parser.o: parser.cpp
	g++ $(CFLAGS) parser.cpp

main.o: main.cpp
	g++ $(CFLAGS)  main.cpp

lexer.o: lexer.cpp
	g++ $(CFLAGS) lexer.cpp

interpreter.o: interpreter.cpp
	g++ $(CFLAGS) interpreter.cpp

executor.o: executor.cpp
	g++ $(CFLAGS) executor.cpp

libpscmd.so: pscmd.o
	g++ -shared -o libpscmd.so pscmd.o

pscmd.o: pscmd.cpp
	g++ $(CFLAGS) -fPIC pscmd.cpp

clean:
	rm -rf *.o shell libpscmd.so


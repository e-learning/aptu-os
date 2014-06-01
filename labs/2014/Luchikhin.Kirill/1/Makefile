default: shell

shell: main.o shell.o directory.o utils.o
	g++ main.o shell.o directory.o utils.o -o shell

main.o: main.cpp
	g++ -c main.cpp

shell.o: shell.cpp
	g++ -c shell.cpp

directory.o: directory.cpp
	g++ -c directory.cpp

utils.o: utils.cpp
	g++ -c utils.cpp

clean:
	rm -rf *.o shell

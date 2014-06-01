default:  clean shell
main.o: main.cpp 
	g++ -c -Wall -Wextra -pedantic -std=c++0x   main.cpp  
shell: main.o 
	g++ main.o -o shell
clean:
	rm -rf *.o shell



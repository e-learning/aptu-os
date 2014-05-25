default:  clean memory
main.o: main.cpp 
	g++ -c -pedantic -std=c++11  main.cpp  
memory: main.o 
	g++ main.o -o memory
clean:
	rm -rf *.o memory



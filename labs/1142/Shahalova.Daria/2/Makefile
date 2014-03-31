shell: main.o ps.so
	g++ main.o -o shell -ldl -Wl,-rpath,.

main.o: main.cpp
	g++ -c main.cpp
	
ps.so: ps.o
	gcc -shared -o ps.so ps.o  
	
ps.o: ps.cpp ps.h
	g++ -c -fPIC ps.cpp -o ps.o  

clean:
	rm -rf *.o shell


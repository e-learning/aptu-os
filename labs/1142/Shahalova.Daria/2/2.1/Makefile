shell: main.o libps.so
	g++ main.o -o shell -L. -lps -Wl,-rpath,.

main.o: main.cpp
	g++ -c main.cpp
	
libps.so: ps.o
	gcc -shared -o libps.so ps.o  
	
ps.o: ps.cpp ps.h
	g++ -c -fPIC ps.cpp -o ps.o  

clean:
	rm -rf *.o shell


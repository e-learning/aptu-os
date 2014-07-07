all:exe_server exe_client
	./server

exe_server:main_s.o
	g++ -L. -pthread -o server  main_s.o -Wl,-rpath,.

main_s.o:main_s.cpp
	g++ -c main_s.cpp -o main_s.o

exe_client:main_c.o
	g++ -L. -pthread -o client main_c.o -Wl,-rpath,.

main_c.o:main_c.cpp
	g++ -c main_c.cpp -o main_c.o

clean:*.o
	rm *.o

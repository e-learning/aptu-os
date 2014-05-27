all:cli ser 
	g++ -L. -pthread -o client client.o -Wl,-rpath,.
	g++ -L. -pthread -o server server.o -Wl,-rpath,.
cli:client.cpp
	g++ -c -o client.o client.cpp
ser:server.cpp
	g++ -c -o server.o server.cpp

all:client server
	g++ -L. -pthread -o client client.o -Wl,-rpath,.
	g++ -L. -pthread -o server server.o -Wl,-rpath,.
client:client.cpp
	g++ -c -o client.o client.cpp
server:server.cpp
	g++ -c -o server.o server.cpp



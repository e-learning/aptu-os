chat: client server

server: server.o
	g++ server_main.o -o server -lpthread

server_main.o: server.cpp
	g++ -c server.cpp
	
client: client_main.o
	g++ client_main.o -o client -lpthread

client_main.o: client.cpp
	g++ -c client.cpp

clean:
	rm -rf *.o allocator


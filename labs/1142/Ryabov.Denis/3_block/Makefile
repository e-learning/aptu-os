chat: client server

server: server_main.o
	g++ server_main.o -o server -lpthread

server_main.o: server_main.cpp
	g++ -c server_main.cpp
	
client: client_main.o
	g++ client_main.o -o client -lpthread

client_main.o: client_main.cpp
	g++ -c client_main.cpp

clean:
	rm -rf *.o allocator


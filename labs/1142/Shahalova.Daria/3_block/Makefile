chat: main.o Server.o Client.o
	g++ main.o Server.o Client.o -o chat -lpthread
	
main.o: main.cpp
	g++ -c main.cpp
	
Server.o: Server.cpp Server.h
	g++ -c Server.cpp
	
Client.o: Client.cpp Client.h
	g++ -c Client.cpp

clean:
	rm -rf *.o chat


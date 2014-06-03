make: Client.o Server.o
	g++ -L. -pthread -o Client Client.o -Wl,-rpath,.
	g++ -L. -pthread -o Server Server.o -Wl,-rpath,.
Client.o:Client.cpp
	g++ -c -o Client.o Client.cpp
Server.o:Server.cpp
	g++ -c -o Server.o Server.cpp  

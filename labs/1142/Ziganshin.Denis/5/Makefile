make: client.o serv.o
	g++ -L. -pthread -o client client.o -Wl,-rpath,.
	g++ -L. -pthread -o serv serv.o -Wl,-rpath,.
client.o:client.cpp
	g++ -c -o client.o client.cpp
serv.o:serv.cpp
	g++ -c -o serv.o serv.cpp  

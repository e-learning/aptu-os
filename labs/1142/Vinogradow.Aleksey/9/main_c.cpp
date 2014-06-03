///////////////////////////////////////////
//                                       //
//        Vin92.23.05.2014               //
//                     client            //
//                                       //
///////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>

int sock;

using namespace std;

void* potok(void *){
	char buffer[255];	

	while(1){

	for(int i=0; i< 255; i++) buffer[i] = 0;
		if(recv(sock, buffer, 255, 0)<0){
			cout<<"ERROR"<<endl;
			break;
		}else{
			if (buffer[0]!= 0)
				cout<<buffer<<endl;
		}
	sleep(1);
	}
}

int main(){

	cout<<endl<<"Create socket: ";
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock > 0){
		cout<<"OK"<<endl;
	}else{
		cout<<"ERR"<<endl;
		return -1;
	}

/////////////////////////////////////////////////////////////////
	in_addr ip_addr;

	string ip_addr_s;
	cout<<"ip: ";
	cin>>ip_addr_s;
//	ip_addr_s = "127.0.0.1";

	ip_addr.s_addr = inet_addr(ip_addr_s.c_str());

	if (ip_addr.s_addr!=INADDR_NONE){
		cout<<"OK"<<endl;
	}else{
		cout<<"ERR"<<endl;
		return -2;
	}


/////////////////////////////////////////////////////////////////
	sockaddr_in addr;

	int port;
	cout<<"Port(3425):";
	cin>>port;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = ip_addr;

	
	cout<<"Connect to "<<ip_addr_s<<"...";

	if(!connect(sock, (struct sockaddr *)&addr, sizeof(addr) )){
		cout<<"OK"<<endl;
	}else{
		cout<<"ERR"<<endl;
		return -3;
	}

/////////////////////////////////////////////////////////////////

	pthread_t thread;
	pthread_create(&thread, NULL, potok, 0);
	string command="";
	char buffer[255];




	while(1){
		while (command == "") getline(cin,command);
		
		send(sock, command.c_str(), command.length(), 0);

		if (command == "exit") break;

		if (command == "history"){
			for(int i=0; i< 255; i++) buffer[i] = 0;
				recv(sock, buffer, 255, 0);
				cout<<buffer<<endl;
		}



		command = "";


	}



	return 0;
}

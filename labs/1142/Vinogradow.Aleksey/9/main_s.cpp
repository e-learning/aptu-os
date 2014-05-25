///////////////////////////////////////////
//                                       //
//        Vin92.23.05.2014               //
//                     server            //
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
#include <sstream>

#define COLL_USERS 100

int sock;
int users[COLL_USERS];
int new_user;
std::string line;

using namespace std;

void send_all(string str, int  zn2){
	for(int i=0; i<COLL_USERS; i++){
	
	if ((users[i]>0)&& (i!=zn2)){
		send(users[i], str.c_str(), str.length(), 0);
		}
	}
}

void* potok2(void *){
	int my_nom = new_user;

	while(1){
		char buffer[255];
		for (int i1=0; i1<255; i1++) buffer[i1] = 0;
			
		int rez= recv(users[my_nom], buffer, 255, 0); 

		ostringstream oss;
		oss<<"u"<<my_nom+1<<": "<<buffer;
	
		string tmp = buffer;//oss.str();//"u" + itoa(my_nom) + ":" + buffer;
		if (tmp == "exit") rez = -1;

//		if (tmp == "history"){ send(users[my_nom], line.c_str(), line.length(), 0); rez = 0;}

		if(rez<0){
			users[my_nom] = -1;
			cout<<"del user "<<my_nom + 1<<endl;
			break;
		}

		if(rez){			
			cout<<"["<<my_nom+1<<"]"<<buffer<<endl;
			line += oss.str();
			line += "\n\r";
			send_all(oss.str(), my_nom);	
		} 


	}
}

void* potok(void *){

	pthread_t thread;
	while(1){
		int i = 0;
		while(users[i] != -1) i++;
		users[i] = accept(sock, NULL, NULL);
		cout<<"user "<<i+1<<endl;
		new_user = i;
		pthread_create(&thread, NULL, potok2, 0);
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
	cout<<"ip(127.0.0.1): ";
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

	
	cout<<"bind "<<ip_addr_s<<"...";

	if(!bind(sock, (struct sockaddr *)&addr, sizeof(addr) )){
		cout<<"OK"<<endl;
	}else{
		cout<<"ERR"<<endl;
		return -3;
	}

	cout<<"listen..."<<endl;;
/////////////////////////////////////////////////////////////////
	pthread_t thread;

	listen(sock, 1);


	for(int i = 0; i < COLL_USERS; i++) users[i] = -1;

	pthread_create(&thread, NULL, potok, 0);


	string command = "";

	while(1){
		while (command == "") getline(cin,command);

		if (command == "stop") break;

		if (command == "users"){
	
			for(int i=0; i < COLL_USERS; i++){	
				if(users[i] != -1)
				cout<<"user "<<i+1<<endl;
	
			}

		}

		command = "";
	}

	close(sock);

	return 0;
}

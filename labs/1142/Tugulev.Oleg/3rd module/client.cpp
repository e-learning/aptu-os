#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>

#define PORT 3425
bool flag = false;
using namespace std;
void *reciever(void* arg)
{
	char mes[255];
	int *socket;
	socket = (int*)arg;
	while(1)
	{
		/*for (int i=0; i<255; i++)
			{
				mes[i]=0;
			}*/
		recv(*socket,mes,255,0);
		cout<<mes<<endl;
		sleep(1);
	}
	return NULL;
}


void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main()
{	pthread_t thr_handler;
	struct sockaddr_in adr_st;
	struct in_addr in_ad;
	string message;
	string user_name;
	bool run=true;
	int socket_h;

	do
	{
		user_name.clear();
		cout<<"What is your name?(max 25 chr): ";
        	cin>>user_name;
	}
	while(user_name.length()>25);

	//create socket
	socket_h=socket(AF_INET,SOCK_STREAM,0); // (adr family, type of socket, protocol)
	if (socket_h<0)
		error("Can't create socket");
	//struct filling
	in_ad.s_addr=inet_addr("127.0.0.1");
	adr_st.sin_family = AF_INET;
	adr_st.sin_port = htons(PORT); // convert to network order of bytes
	adr_st.sin_addr = in_ad;
	//connect to server
	if(connect (socket_h,(struct sockaddr *)&adr_st,sizeof(adr_st))<0)
		error("Can't connect!");
	cout<<"Connected!"<<endl<<"You can send messages"<<endl<<"Type STOP_CHAT to stop working"<<endl;
	//recive messages from server
	pthread_create(&thr_handler,NULL,reciever,(void*)&socket_h);
	//ready to work!
	//send messges to server
	while (1)
	{	
		string work;
		getline(cin,work);
		message=user_name+":"+work;
		
		if(send(socket_h,message.c_str(),message.length(),0)<0)
				error("Can't send!");
		message.clear();
		if (work=="STOP_CHAT")
			break;
		work.clear();

		
	}
	//end
	close(socket_h);
	return 0;
}

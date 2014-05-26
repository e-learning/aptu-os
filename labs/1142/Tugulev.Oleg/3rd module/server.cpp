#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <sstream>

#define PORT 3425

using namespace std;

struct who_online
{
	int id;
	int socket;
};
vector<who_online> data;
int iter=1;

void *recieve_and_send(void* inp)
{
	who_online *user;
	user = (who_online*) inp;
	char mes[255];
	int sock = user->socket;
	int vec_el=user->id;
	stringstream tmp;
	string buffer;
	while(1)
	{
		recv(sock,mes,255,0);
		tmp<<mes;
		tmp>>buffer;
		size_t found=buffer.find(':');
		string com=buffer.substr(found);
		if (com.compare("ST")==0)
			{
				close(sock);
				iter--;
				for (int i=vec_el;i<data.size();i++)
				{
					data[i]=data[i+1];
					data[i].id--;
				}
				data.pop_back();
				break;
			}
		for (int i=0;i<iter;i++)
			send(data[i].socket,buffer.c_str(),buffer.length(),0);
	
	}
	return NULL;
}


void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main()
{	
	pthread_t thr_handler;
	struct sockaddr_in adr_st;
	int server_h,socket_h;
        who_online cont;

	//create socket
	server_h=socket(AF_INET,SOCK_STREAM,0); // (adr family, type of socket, protocol)
	if (server_h<0)
		error("Can't create socket");
	//struct filling
	adr_st.sin_family = AF_INET;
	adr_st.sin_port = htons(PORT); // convert to network order of bytes
	inet_pton(AF_INET,"127.0.0.1",&adr_st.sin_addr);//(type of adr,adr as char,destination)
	//socket binding
	if(bind(server_h,(struct sockaddr*)&adr_st,sizeof(adr_st))<0)
		error("Can't bind!");
	//listen
	listen (server_h,1);
	//ready to work!
	while(iter!=0)
	{
		iter--;
		socket_h=accept(server_h,NULL,NULL);
		cout<<"+1";
		cont.id=iter;
		iter++;
		cont.socket=socket_h;
		data.push_back(cont);
		pthread_create(&thr_handler,NULL,recieve_and_send,(void*)&data[iter-1]);
		
	}
	
	//end
	close(server_h);
	return 0;
}

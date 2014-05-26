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
	printf("%i ",user->id);
	printf("%i ",user->socket);
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
		printf("%s",mes);
		size_t found=buffer.find(':');
		string com=buffer.substr(found);
		if (com.compare("ST")==0)
			{
			  write(sock,"disconnected",12);
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
		else
		        for (int i=0;i<iter;i++)
			write(data[i].socket,buffer.c_str(),buffer.length());
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
	printf("do\n");
	listen (server_h,1);
        printf("posle\n");
	//ready to work!
	while(iter!=0)
	{
		iter--;
		printf("%i",iter);
		socket_h=accept(server_h,NULL,NULL);
		if (write(socket_h,"I got your message",18)<0)
		  error("Can't send to client!");
		cont.id=iter;
		iter++;
		cont.socket=socket_h;
		data.push_back(cont);
		printf("socket %i\n",data[iter-1].socket);
		printf("data %i\n",data[iter-1].id);
		if(pthread_create(&thr_handler,NULL,recieve_and_send,(void*)&data[iter-1])!=0)
		  error("Can't create thread!");
		else
		  printf("OK\n");
		
	}
	
	//end
	close(server_h);
	return 0;
}

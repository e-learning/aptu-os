#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <cstring>

#define PORT 3425

using namespace std;

struct to_func
{
	int id;
	int socket;
};
int *list;
int iter=0;
int current_client;
struct to_func cont;

void *recieve_and_send(void* )
{	
  //	printf("%i ",data[current_client].id /*user->id*/);
  //	printf("%i ",data[current_client].socket/*user->socket*/);
	char mes[255];
	int sock = cont.socket;
	list[cont.id-1]=sock;
	int el = cont.id-1 ;
	while(1)
	{
		recv(sock,mes,255,0);
		if (strstr(mes,"STOP_CHAT")!=NULL)
			{
				close(sock);
				//printf("%i",vec_el);
				for (int i=el;i<iter;i++)
					list[i]=list[i+1];
				list[iter-1]=0;
				iter--;
				
				break;
			}

		
		for (int i=0;i<iter;i++)

		      if (i!=el)
			{
			  //printf("message %s\n",buffer.c_str());
				write(list[i],mes,255);
			}
		for (int i=0;i<255;i++)
			mes[i]=0;
		sleep(2);
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
	bool run=true;
	int cap=4;
	list=(int*)malloc(sizeof(int)*cap);

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
	while(run)
	{
		socket_h=accept(server_h,NULL,NULL);
		cont.id=iter;
		iter++;
		current_client=iter-1;
		if(iter>=cap)
			{
				cap++;
				list = (int*)realloc(list,sizeof(int)*cap);
			}
		cont.socket=socket_h;
		cont.id=iter;
		if(pthread_create(&thr_handler,NULL,recieve_and_send,NULL/*(void*)&data[iter-1]*/)!=0)
		  error("Can't create thread!");
		else
		  printf("OK\n");
	
		if (iter==0)
			run=false;		
	}
	printf("konec!");
	//end
	close(server_h);
	return 0;
}

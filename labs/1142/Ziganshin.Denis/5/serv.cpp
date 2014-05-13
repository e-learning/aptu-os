#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#define MESS_SIZE 100

int *usrs;
int kolUsr;
int SizeOfUsrs;


struct args{
	int sock;
	int num;
};

void* client(void* arg){
	int i,socket,num;
	char st[MESS_SIZE];
	//==============переопределение=====
	struct args *a;
	a = (args*) arg;
	socket = a->sock;
	num = a->num-1;
	usrs[num] = socket;
	//==================================
	while(1){
		recv(socket,st,MESS_SIZE,0);
		if(strstr(st,"EXIT") != NULL){
			close(socket);
			for(i=num;i<kolUsr-1;i++)
				usrs[i] = usrs[i+1];
			usrs[kolUsr-1] = 0;
			kolUsr--;
			break;
		}
		for(i=0;i<kolUsr;i++){
			if(i != (num))
				send(usrs[i],st,MESS_SIZE,0);
		}
	}
	return NULL;
}

int main(){
	int s,sock,start;
    struct sockaddr_in addr;
	pthread_t thread;
	struct args a;
	
	
    s = socket(AF_INET, SOCK_STREAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425); 
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
    
    SizeOfUsrs = 10;
    usrs = (int*)malloc(sizeof(int)*SizeOfUsrs);
    
    bind(s,(struct sockaddr *)&addr,sizeof(addr));
    listen(s,1);
    kolUsr = 0;
    start = 1;
    while(kolUsr+start){
		sock=accept(s,NULL,NULL);
		kolUsr++;
		start = 0;
		if(kolUsr >= SizeOfUsrs){
			SizeOfUsrs *= 2;
			usrs = (int*)realloc(usrs,sizeof(int)*SizeOfUsrs);
		}
		a.sock = sock;
		a.num = kolUsr;
		pthread_create(&thread, NULL ,client,(void*)(&a));
	}
    close(s);
    return 0;
}


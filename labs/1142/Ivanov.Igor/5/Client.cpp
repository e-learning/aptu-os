#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 3425
#define MESS_SIZE 100

void* recv_mess(void* s){
	char st[MESS_SIZE];
	int *sock;
	sock = (int*)s;
	while(1)
	{
		recv(sock[0],st,MESS_SIZE,0);
		st[MESS_SIZE] = 0;
		printf("%s\n",st);
	}
	return NULL;
}

int main(){
	int s;
	char st[MESS_SIZE];
    	struct sockaddr_in addr;
	pthread_t thread;
	
    	s = socket(AF_INET, SOCK_STREAM, 0);
    
    	addr.sin_family = AF_INET;
    	addr.sin_port = htons(PORT); 
    	inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
    
    	if(connect(s, (struct sockaddr *)&addr, sizeof(addr))<0)
	{
		printf("error: %s\n",strerror(errno));
		return 2;
	}

    	pthread_create(&thread, NULL ,recv_mess,(void*)&s);
    	while(1)
	{
		scanf("%s",&st[0]);
		send(s,st,MESS_SIZE,0);
		if(strstr(st,"EXIT") != NULL)
			break;
	}
    	close(s);
    	return 0;
}


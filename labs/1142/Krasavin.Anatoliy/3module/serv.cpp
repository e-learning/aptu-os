#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



int main(){
	int s,i;
	unsigned int len;
	char st[100];
	char message[100];
    struct sockaddr_in addr;
		
    s = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3426); 
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
    len = sizeof(addr);
    bind(s,(struct sockaddr *)&addr,sizeof(addr));
    recvfrom(s,message,100,0,(struct sockaddr *)&addr,(socklen_t *)&len);
    printf("recv:%s\n",message);
    len = strlen(message);
    for(i=0;i<len;i++){
		st[i] = message[len-i-1];
	}
	st[len] = 0;
	printf("%s\n",st);
    sendto(s,st,100,0,(struct sockaddr *)&addr,sizeof(addr));    
    close(s);
    return 0;
}


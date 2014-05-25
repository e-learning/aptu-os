#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



int main(){
	int s;
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
    printf("string:\n");
    scanf("%s",&st[0]);
    sendto(s,st,100,0,(struct sockaddr *)&addr,sizeof(addr));
    recvfrom(s,message,100,0,(struct sockaddr *)&addr,(socklen_t *)&len);
    printf("answer: %s\n",message);
    
    close(s);
    return 0;
}


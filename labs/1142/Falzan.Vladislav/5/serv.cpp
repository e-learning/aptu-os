#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



int main(){
	int sock,i;
	unsigned int len;
	char st[30];
	char mes[30];
	char ex[10];
    struct sockaddr_in addr;
		
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3426); 
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
    len = sizeof(addr);
    bind(sock,(struct sockaddr *)&addr,sizeof(addr));
while (strstr(&ex[0],"exit") == NULL)
{
    recvfrom(sock,mes,30,0,(struct sockaddr *)&addr,(socklen_t *)&len);
    printf("Prinyato:%s\n",mes);
    len = strlen(mes);
    for(i=0;i<len;i++){
		st[i] = mes[len-i-1];
	}
	st[len] = 0;
	printf("Perevorot:%s\n",st);
    sendto(sock,st,30,0,(struct sockaddr *)&addr,sizeof(addr));
    scanf("%s",&ex[0]);
}    
    close(sock);
    return 0;
}

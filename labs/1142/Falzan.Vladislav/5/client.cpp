#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



int main(){
	int sock;
	unsigned int len;
	char st[30];
	char mes[30];
    struct sockaddr_in addr;
		
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3426); 
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
    len = sizeof(addr);
    bind(sock,(struct sockaddr *)&addr,sizeof(addr));
    printf("Vvod stroki:");
    scanf("%s",&st[0]);
    while (strstr(&st[0],"exit") == NULL)
{
    sendto(sock,st,30,0,(struct sockaddr *)&addr,sizeof(addr));
    recvfrom(sock,mes,30,0,(struct sockaddr *)&addr,(socklen_t *)&len);
    printf("Otvet:%s\n",mes);
    scanf("%s",&st[0]);
}    
    close(sock);
    return 0;
}


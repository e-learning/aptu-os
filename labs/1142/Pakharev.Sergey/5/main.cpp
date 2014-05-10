#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define BUF_SIZE 2048
#define CMD_SIZE 50
char buf[BUF_SIZE];
int sock;
void Print_inform();
void Send_Rec(char input_str[CMD_SIZE]);

int main(int argc, char **argv)
{
	struct sockaddr_in addr;
	struct hostent* host_addr;
	
	Print_inform();
	host_addr = gethostbyname("pop.mail.ru");
	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock<0)
	{
		perror("socket");
		exit(1);
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(110);
	memcpy(&addr.sin_addr,host_addr->h_addr_list[0],host_addr->h_length);
	
	if (connect(sock,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		perror("connect");
		exit(1);
	}
	recv(sock,buf,BUF_SIZE,0);
	sleep(2);	
	printf("***CONNECTED***\n\r");
	
	char input_cmd[CMD_SIZE];	
	char* input_pas;
	
	printf("Please, enter your login:\n\r");
	memset(buf,'\0',BUF_SIZE);
	strcat(buf,"USER ");
	gets(input_cmd);
	strcat(buf,input_cmd);
	strcat(buf,"\n\r");	
	Send_Rec(buf);
	
	memset(buf,'\0',BUF_SIZE);
	strcat(buf,"PASS ");
	input_pas = getpass("Please, enter your password:\n\r");
	strcat(buf,input_pas);
	strcat(buf,"\n\r");
	Send_Rec(buf);
	
	while (true)
	{
		memset(input_cmd,'\0',CMD_SIZE);
		gets(input_cmd);
		strcat(input_cmd,"\n\r");
		Send_Rec(input_cmd);
	}
	close(sock);
	
	return 0;
}

void Print_inform()
{
	printf("****Using of pop3_client****\n\r");
	printf(" Client commands:\n\r");
	printf("  -USER user_name\n\r");
	printf("  -PASS user_pass\n\r");
	printf("  -LIST (n)\n\r");
	printf("  -TOP message_num lines_count\n\r");
	printf("  -NOOP\n\r");
	printf("  -RETR message_num\n\r");
	printf("  -STAT\n\r");
	printf("  -RSET\n\r");	
	printf("  -QUIT\n\r");
	printf(" \n\r");
}

void Send_Rec(char input_str[CMD_SIZE])
{
	send (sock, input_str,strlen(input_str),0);
	memset(buf,'\0',BUF_SIZE);
	sleep(2);
	recv(sock,buf,BUF_SIZE,0);
		
	printf("Returned value: ");
	printf(buf);
}

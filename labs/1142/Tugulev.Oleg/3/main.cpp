#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define PORT 80;

using namespace std;

void error(const char *msg)
{
	perror(msg);
	exit(0);
}



void main()
{
struct sockaddr adr_st;
string message;
bool run=true;
int socket_h;
	//create socket
	socket_h=socket(AF_INET,SOCK_STREAM,0); // (adr family, type of socket, protocol)
	if (socket_h<0)
		error("Can't create socket");
	//struct filling
	adr_st.sin_family = AF_INET;
	adr_st.sin_port = htons(PORT); // convert to network order of bytes
	inet_pton(AF_INET,"127.0.0.1",&adr_st.sin_addr);//(type of adr,adr as char,destination)
	//connect to server
	if(connect (socket_h,(struct sockaddr*),sizeof(adr_st))<0)
		error("Can't connect!");
	cout<<"Connected!"<<endl<<"You can send messages"<<endl<<"Type ST to stop working";
	//ready to work!
	while (run)
	{	
	cout <<endl<<"@";
	cin >>message;
	if (message.compare("ST") != 0)
		run = false;
	else
		if(send(socket_h,message.c_str(),message.length(),0)<0)
			error("Can't send!");
	}
	//end
	close(socket_h);
}

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>

#define PORT 3425
#define size 100

bool flag = false;
using namespace std;
void *rec(void* arg)
{
        char buf[size];
        int *sock;
        sock = (int*)arg;
        while(1)
        {
                recv(*sock,buf,size,0);
                cout<<buf<<endl;
        }
        return NULL;
}


void error(const char *msg)
{
        perror(msg);
        exit(0);
}

int main()
{       pthread_t thread;
        struct sockaddr_in addr;
        struct in_addr in_ad;
        string message;
        string name;
        bool run=true;
        int soc;

        do
        {
                name.clear();
                cout<<"What is your name?(max 25 chr): ";
                cin>>name;
        }
        while(name.length()>25);

        //create socket
        soc=socket(AF_INET,SOCK_STREAM,0); 
        if (soc<0)
                error("Can't create socket");
        //struct filling
        in_ad.s_addr=inet_addr("127.0.0.1");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT); 
        addr.sin_addr = in_ad;
        //connect to server
        if(connect (soc,(struct sockaddr *)&addr,sizeof(addr))<0)
                error("Can't connect!");
        cout<<"Connected!\n";
	cout<<"You can send messages\n";       
        pthread_create(&thread,NULL,rec,(void*)&soc);
        while (1)
        {      
                string work;
                getline(cin,work);
                message=name+":"+work;
               
                if(send(soc,message.c_str(),message.length(),0)<0)
                                error("Can't send!");
                message.clear();
                if (work=="STOP_CHAT")
                        break;
                work.clear();

               
        }
        close(soc);
        return 0;
}



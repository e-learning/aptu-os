#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <cstring>

#define PORT 3425
#define size 100

using namespace std;
struct fsc
{
        int num;
        int socket;
};
int *list;
int counter=0;
int client;
struct fsc s;
void *recuv(void* )
{      
        char buf[size];
        int sockt = s.socket;
        list[s.num-1]=sockt;
	int element = s.num-1;
        while(1)
        {
                recv(sockt,buf,size,0);
                if (strstr(buf,"STOP_CHAT")!=NULL)
                        {
                                close(sockt);
                                for (int i=element;i<counter;i++)
                                        list[i]=list[i+1];
                                list[counter-1]=0;
                                counter--;                               
                                break;
                        }               
                for (int i=0;i<counter;i++)

                      if (i!=element)
                        {                          
                                write(list[i],buf,size);
                        }
                for (int i=0;i<size;i++)
                        buf[i]=0;
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
	      pthread_t thread;
        struct sockaddr_in addr;
        int serv,soc;
        bool run=true;
        int cap=4;
        list=(int*)malloc(sizeof(int)*cap);

        serv=socket(AF_INET,SOCK_STREAM,0);
        if (serv<0)
                error("Can't create socket");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT); 
        inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
        if(bind(serv,(struct sockaddr*)&addr,sizeof(addr))<0)
                error("Can't bind!");
        listen (serv,1);
       
        while(run)
        {
                soc=accept(serv,NULL,NULL);
                s.num=counter;
                counter++;
                client=counter-1;
                if(counter>=cap)
                        {
                                cap++;
                                list = (int*)realloc(list,sizeof(int)*cap);
                        }
                s.socket=soc;
                s.num=counter;
                if(pthread_create(&thread,NULL,recuv,NULL)!=0)
                  error("Can't create thread!");
                else
                  printf("OK\n");
       
                if (counter==0)
                        run=false;              
        }
        printf("konec!");
        //end
        close(serv);
        return 0;
}


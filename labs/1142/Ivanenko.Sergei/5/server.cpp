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
//struct to_func
{
        int num;
        int socket;
};
int *list;
int counter=0;
//int iter=0;
int client;
//struct to_func cont;
struct fsc s;
void *recuv(void* )
{      
  //    printf("%i ",data[current_client].id /*user->id*/);
  //    printf("%i ",data[current_client].socket/*user->socket*/);
        char buf[size];
        int sockt = s.socket;
        list[s.num-1]=sockt;
        //int el = cont.id-1 ;
	int element = s.num-1;
        while(1)
        {
                recv(sockt,buf,size,0);
                if (strstr(buf,"STOP_CHAT")!=NULL)
                        {
                                close(sockt);
                                //printf("%i",vec_el);
                                for (int i=element;i<counter;i++)
                                        list[i]=list[i+1];
                                list[counter-1]=0;
                                counter--;
                               
                                break;
                        }

               
                for (int i=0;i<counter;i++)

                      if (i!=element)
                        {
                          //printf("message %s\n",buffer.c_str());
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
       // pthread_t thr_handler;
        struct sockaddr_in addr;
        int serv,soc;
        bool run=true;
        int cap=4;
        list=(int*)malloc(sizeof(int)*cap);

        //create socket
        serv=socket(AF_INET,SOCK_STREAM,0); // (adr family, type of socket, protocol)
        if (serv<0)
                error("Can't create socket");
        //struct filling
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT); // convert to network order of bytes
        inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);//(type of adr,adr as char,destination)
        //socket binding
        if(bind(serv,(struct sockaddr*)&addr,sizeof(addr))<0)
                error("Can't bind!");
        //listen
        listen (serv,1);
        //ready to work!
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


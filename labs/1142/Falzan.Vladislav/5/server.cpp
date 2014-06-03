#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <cstring>

#define PORT 3425

using namespace std;

struct to_func
{
        int id;
        int socket;
};
int *list;
int iter=0;
int current_client;
struct to_func cont;

void *recieve_and_send(void* )
{      
        char mes[255];
        int sock = cont.socket;
        list[cont.id-1]=sock;
        int el = cont.id-1 ;
        while(1)
        {
                recv(sock,mes,255,0);
                if (strstr(mes,"STOP")!=NULL)
                        {
                                close(sock);
                                for (int i=el;i<iter;i++)
                                        list[i]=list[i+1];
                                list[iter-1]=0;
                                iter--;
                               
                                break;
                        }

               
                for (int i=0;i<iter;i++)

                      if (i!=el)
                        {
                                write(list[i],mes,255);
                        }
                for (int i=0;i<255;i++)
                        mes[i]=0;
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
        pthread_t thr_handler;
        struct sockaddr_in adr_st;
        int server_h,socket_h;
        bool run=true;
        int cap=4;
        list=(int*)malloc(sizeof(int)*cap);

        server_h=socket(AF_INET,SOCK_STREAM,0);
        if (server_h<0)
                error("Can't create socket");
        adr_st.sin_family = AF_INET;
        adr_st.sin_port = htons(PORT);
        inet_pton(AF_INET,"127.0.0.1",&adr_st.sin_addr);
        if(bind(server_h,(struct sockaddr*)&adr_st,sizeof(adr_st))<0)
                error("Can't bind!");
        listen (server_h,1);
        while(run)
        {
                socket_h=accept(server_h,NULL,NULL);
                cont.id=iter;
                iter++;
                current_client=iter-1;
                if(iter>=cap)
                        {
                                cap++;
                                list = (int*)realloc(list,sizeof(int)*cap);
                        }
                cont.socket=socket_h;
                cont.id=iter;
                if(pthread_create(&thr_handler,NULL,recieve_and_send,NULL)!=0)
                  error("Can't create thread!");
                else
                  printf("Est' kontakt!\n");
       
                if (iter==0)
                        run=false;              
        }
        printf("End!");
        close(server_h);
        return 0;
}

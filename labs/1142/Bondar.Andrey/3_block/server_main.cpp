#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <list>

using namespace std;

typedef struct
{
    vector<int> *participants;
    int clientSock;
}serverContext;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int listener;
list<pthread_t *> threads;
vector<int> participants;

void *clientProcessingCycle(void *arg)
{
    char buf[1024];
    int bytesRead;
    serverContext *sCtx;
    sCtx = (serverContext *) arg;
    
    while (1) {
        bytesRead = recv(sCtx->clientSock, buf, 1024, 0);
        for (int i = 0 ; i < sCtx->participants->size() ; i++)
            if (sCtx->participants->at(i) != sCtx->clientSock) {
                send(sCtx->participants->at(i), buf, bytesRead, 0);
            }
    }
}

void *serverCycle(void *arg)
{
    int sock;
    struct sockaddr_in addres;
    pthread_t *newClientThread;
    serverContext *sCtx;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("Socket Error");
        exit(1);
    }
    
    addres.sin_family = AF_INET;
    addres.sin_port = htons(8000);
    addres.sin_addr.s_addr = INADDR_ANY;
    if (bind(listener, (struct sockaddr *)&addres, sizeof(addres)) < 0) {
        perror("Bind Error");
        exit(1);
    }

    listen(listener, 1);
    
    while (1) {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("Accept Error");
            exit(1);
        }
        
        participants.push_back(sock);
        sCtx = new serverContext;
        pthread_mutex_lock(&mutex);
        sCtx->participants = &participants;
        pthread_mutex_unlock(&mutex);
        sCtx->clientSock = sock;
        
        newClientThread = new pthread_t;
        threads.push_back(newClientThread);
        pthread_create(newClientThread, NULL, clientProcessingCycle, (void *) sCtx);
    }
    
    close(listener);
}

int main(int argc, char** argv)
{
    pthread_t mainServerThread;
    string inputStr;
    
    pthread_create(&mainServerThread, NULL, serverCycle, (void *) NULL);
    
    while (1) {
        cout << ">";
        getline(cin, inputStr);
        
        if (inputStr == "exit") {
            break;
        }
    }
    
    pthread_cancel(mainServerThread);
    
    for (vector<int>::iterator it = participants.begin(); it != participants.end(); it++)
        close(*it);
    
    for (list<pthread_t *>::iterator it = threads.begin(); it != threads.end(); it++)
        pthread_cancel(*(*it));
    
    pthread_cancel(mainServerThread);
    
    close(listener);
    
    return 0;
}

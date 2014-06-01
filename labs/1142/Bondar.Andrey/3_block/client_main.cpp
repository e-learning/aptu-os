#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>

using namespace std;

#define NAME_SIZE 50
#define READ_BUFFER_SIZE 1024

typedef struct
{
   int sock;
   char name[NAME_SIZE]; 
}clientContext;

void *recvThreadFunc(void *arg)
{
    char buf[READ_BUFFER_SIZE];
    clientContext *cCtx;
    int messageSize;
    char *strBuf;
    int bytesParsed;
    int n;
    int bufIdx;
    
    cCtx = (clientContext *)arg;
    
    bytesParsed = 0;
    messageSize = -1;
    
    while (1) {
        n = recv(cCtx->sock, &buf, READ_BUFFER_SIZE, 0);
        
        if (n > 0) {
            bufIdx = 0;
            while (bufIdx < n) {
                if (bytesParsed == 0) {
                    memcpy(&messageSize, &buf[bufIdx], sizeof(int));
                    bytesParsed = sizeof(int);
                    bufIdx = sizeof(int);
                    strBuf = new char[messageSize + 1];
                }
                
                if (bytesParsed > 0 && bytesParsed < messageSize) {
                    int toCpy = (messageSize + sizeof(int) - bytesParsed < n)? 
                        messageSize + sizeof(int) - bytesParsed : n;
                    memcpy(&strBuf[bytesParsed - sizeof(int)], &buf[bufIdx],toCpy);
                    bytesParsed += toCpy;
                    bufIdx += toCpy;
                }
                
                if (bytesParsed == messageSize + sizeof(int)) {
                    char name[NAME_SIZE];
                    memcpy(name, strBuf, NAME_SIZE);
                    strBuf[messageSize] = '\0';
                    cout << name << ":";
                    cout << &strBuf[NAME_SIZE] << endl;
                    delete[] strBuf;
                    bytesParsed = 0;
                }
            }
        }
    }
}

int sendBuf(int s, char *buf, int len, int flags)
{
    int total = 0;
    int n;
    
    while (total < len) {
        n = send(s, buf + total, len - total, flags);
        if (n == -1) { break; }
        total += n;
    }

    return (n==-1 ? -1 : total);
}

void *sendThreadFunc(void *arg)
{
    string inputStr;
    char *buf;
    clientContext *cCtx;
    int bufSize;
    int messageSize;
    
    cCtx = (clientContext *)arg;
    
    while (1) {
        getline(cin, inputStr);
        
        if(inputStr == "exit")
            break;
        messageSize = inputStr.length() + NAME_SIZE;
        bufSize = messageSize + sizeof(int); 
        buf = new char[bufSize];       
        memcpy(&buf[0], &messageSize, sizeof(int));
        memcpy(&buf[sizeof(int)], cCtx->name, NAME_SIZE);
        memcpy(&buf[sizeof(int) + NAME_SIZE], inputStr.c_str(), messageSize - NAME_SIZE);
        if (sendBuf(cCtx->sock, buf, bufSize, 0) < 0) {
            perror("SendBuf Error");
            exit(1);
        }
        delete[] buf;
    }
}

int main() {
    int sock;
    struct sockaddr_in addres;
    pthread_t sendThread, recvThread;
    clientContext cCtx;
    string inputStr;
    
    cout << "Введите адрес сервера" << endl;
    getline(cin, inputStr);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("Socket Error");
        exit(1);
    }

    addres.sin_family = AF_INET;
    addres.sin_port = htons(8000);
    addres.sin_addr.s_addr = inet_addr(inputStr.c_str());
    if(connect(sock, (struct sockaddr *)&addres, sizeof(addres)) < 0)
    {
        perror("Connect Error");
        exit(1);
    }
    
    cCtx.sock = sock;
    
    cout << "Введите имя" << endl;
    getline(cin, inputStr);
    
    strcpy(cCtx.name, inputStr.c_str());
    
    pthread_create(&sendThread, NULL, sendThreadFunc, (void *)&cCtx);
    pthread_create(&recvThread, NULL, recvThreadFunc, (void *)&cCtx);

    pthread_join(sendThread, NULL);
    
    pthread_cancel(recvThread);    
    
    close(sock);

    return 0;
}


#ifndef CLIENT_H
#define	CLIENT_H

#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define USER_NAME_LEN 30

class Client {
public:
    Client();
    virtual ~Client();
    void start();
    static void *read_thread_function(void *arg);
private:
    pthread_t   client_thread;
    char        user_name[USER_NAME_LEN];
    static int  sock;
};

#endif	/* CLIENT_H */


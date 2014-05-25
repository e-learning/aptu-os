#ifndef SERVER_H
#define	SERVER_H

#include <stdlib.h>
#include <set>

using namespace std;

class Server {
public:
    Server();
    virtual ~Server();
    void start();
    static void *thread_function(void *arg);
private:
    pthread_t       server_thread;
    static int      listener;
    static set<int> clients;
};

#endif	/* SERVER_H */


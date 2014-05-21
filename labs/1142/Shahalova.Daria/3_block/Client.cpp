#include "Client.h"
#include <string.h>

int Client::sock; 

Client::Client() {
}

Client::~Client() {
}

void *Client::read_thread_function(void *arg){
    char  buf[1024];
    int   readed;
    int   n;
    char *proc_buf;
    int   mes_size;
    char  name[USER_NAME_LEN];
    
    readed = 0;
    
    while(1)
    {
        if(readed == 0)
        {
            n = recv(sock, &buf, 1024, MSG_PEEK);
            if(n < sizeof(int) + USER_NAME_LEN)
                continue;
            n = recv(sock, &buf, 1024, 0);
            
            memcpy(&mes_size, buf, sizeof(int));
            memcpy(name, &buf[sizeof(int)], USER_NAME_LEN);
            proc_buf = new char[mes_size + 1];
            memcpy(proc_buf, &buf[sizeof(int) + USER_NAME_LEN], n - sizeof(int) - USER_NAME_LEN);
            readed = n;
        }
        else
        {
            n = recv(sock, &buf, 1024, 0);
            memcpy(&proc_buf[readed - sizeof(int) - USER_NAME_LEN], buf, n);     
            readed += n;
        }
                
        if(readed == mes_size + USER_NAME_LEN + sizeof(int))
        {
            proc_buf[mes_size] = '\0';
            cout << name << ": ";
            cout << proc_buf << endl << ">";
            cout.flush();
            readed = 0;
            delete[] proc_buf;
        }
    }
}

void Client::start()
{
    string   command;
    struct   sockaddr_in addr;
    int      n;
    char    *buf;
    int      buf_size;
    int      mes_size;
    
    cout << "Введите IP-адрес сервера" << endl;
    getline(cin, command);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12000);
    addr.sin_addr.s_addr = inet_addr(command.c_str());    
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }
    
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(4);
    }
    
    cout << "Введите имя" << endl;
    getline(cin, command);
    memcpy(&user_name, command.c_str(), command.length());
    
    pthread_create(&client_thread, NULL, &Client::read_thread_function, (void *) NULL);
    
    while(1)
    {
        cout << ">";
        getline(cin, command);
        
        if(command == "exit")
        {
            break;
        }
        else
        {
            mes_size = command.length();
            buf_size = sizeof(int) + USER_NAME_LEN + mes_size;
            buf = new char[buf_size]; 
            memcpy(&buf[0], &mes_size, sizeof(mes_size));
            memcpy(&buf[sizeof(mes_size)], user_name, USER_NAME_LEN);
            memcpy(&buf[sizeof(mes_size) + USER_NAME_LEN], command.c_str(), mes_size);
            
            n = send(sock, buf, buf_size, 0);
            delete[] buf;
        }
    }
    pthread_cancel(client_thread);
    close(sock);
}


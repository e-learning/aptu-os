#include <stdio.h>
#include <iostream>
#include "Server.h"
#include "Client.h"

using namespace std;

int main(int argc, char** argv)
{
    string  command;
    Server  serv;
    Client  cl;
    
    while(1)
    {
        cout << "Введите server для запуска сервера, client для запуска клиента, exit для выхода" << endl;
        getline(cin, command);

        if(command == "server")
        {
            serv.start();
            break;
        }
        
        if(command == "client")
        {
            cl.start();
            break;
        }
        
        if(command == "exit")
            break;
    }
    
    return 0;
}

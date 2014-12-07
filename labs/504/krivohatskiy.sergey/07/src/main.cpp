#include <iostream>
#include <exception>
#include <unistd.h>
#include <string>
#include "traceroot.h"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: traceroute <ip_address>\n";
        return -1;
    }
    if(getuid())
    {
        std::cout << "You must be root\n";
        return -2;
    }

    try
    {
        std::string dest_ip = argv[1];
        traceroot::print(std::cout, dest_ip);
    } catch(std::exception &e) {
        std::cout << e.what() << std::endl;
        return -3;
    }

    return 0;
}

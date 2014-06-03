#include <iostream>
#include <string>
#include "Allocator.h"

int main(int argc, char** argv) {
    Allocator *alloc;
    std::string str;
    size_t size;
    std::string command, arg;
    
    std::cout << "Enter size of memory for allocation:" << std::endl;
    std::cin >> size;
    
    alloc = new Allocator(size);
    
    while(1) {
        std::cin >> command;
        
        if(command == "info") {
            alloc->info();
        }
        
        if(command == "alloc") {
            int res;
            std::cin >> arg;
            if((res = alloc->alloc(atoi(arg.c_str()))) >= 0)
                std::cout << "+" << res << std::endl;
            else
                std::cout << "-" << std::endl;
        }
        
        if(command == "map") {
            alloc->map();
        }
        
        if(command == "free") {
            int res;
            std::cin >> arg;
            if((res = alloc->free(atoi(arg.c_str()))) == 0)
                std::cout << "+" << std::endl;
            else
                std::cout << "-" << std::endl;
        }
        
        if(command == "exit") {
            break;
        }
    }
    
    return 0;
}
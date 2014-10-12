#include "allocator.h"
#include <string>
#include <limits>

int read_number(std::string error_message)
{
    int number;
    while (true)
    {
        std::cin >> number;
        if (!std::cin)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << error_message << std::endl;
        }
        else
        {
            return number;
        }

    }
}


int main()
{
    int N = 0;
    while (true)
    {
        N = read_number("Wrong size of blocks!");
        if (N >= 100 && N <= 10000)
        {
            break;
        }
        else
        {
            std::cout << "Wrong size of blocks!";
        }
    }

    Memory_allocator * memory_allocator = new Memory_allocator(N);
    std::string command;
    while (std::cin >> command)
    {
        if (command == "ALLOC")
        {
            size_t s = 0;
            std::cin >> s;
            if (!std::cin)
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Wrong command" << std::endl;
                continue;
            }
            int alloc_result = memory_allocator->allocate(s);
	    if (alloc_result == -1)
	    {
	        std::cout << "-" << std::endl;
	    }
	    else
	    {
	        std::cout << "+ " << alloc_result << std::endl;
	    }
        }
        else if (command == "FREE")
        {
            int p = 0;
            std::cin >> p;
            if (!std::cin)
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Wrong command" << std::endl;
                continue;
            }
            memory_allocator->free(p);
            std::cout << std::endl;
        }
        else if (command == "INFO")
        {
            memory_allocator->info();
        }
        else if (command == "MAP")
        {
            std::cout << memory_allocator->map();
            std::cout << std::endl;
        }
        else if (command == "EXIT")
        {
            exit(0);
        }
        else
        {
            std::cout << "Unknown command.\nAvailable commands: ALLOC <s>, FREE <p>, MAP, INFO" << std::endl;
        }
    }
    


    getchar();

    return 0;
}
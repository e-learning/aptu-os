#include <iostream>
#include "allocator.h"
int main()
{
    int N;
    std::cin >> N;

    Allocator allocator;
    allocator.init (N);
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "ALLOC") {
            size_t s;
            std::cin >> s;
            size_t res = allocator.alloc (s);
            if (res == 1)
                std::cout << "-" << std::endl;
            else
                std::cout << "+ " <<  res << std::endl;
        } else if (cmd == "FREE") {
            size_t p;
            std::cin >> p;
            size_t res = allocator.free (p);
            if (res == 1)
                std::cout << "-" << std::endl;
            else
                std::cout << "+" << std::endl;
        } else if (cmd == "INFO") {
            allocator.info ();
        } else if (cmd == "MAP") {
            allocator.map ();
        }
    }
    return 0;
}

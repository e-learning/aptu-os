#include <iostream>
#include <string>
#include <cstdlib>
#include "allocator.cpp"


int main() {
  int mem_size;
  std::cin >> mem_size;
  if (mem_size < 100 || mem_size > 10000) {
    std::cout << "Size must be between 100 and 10000" << std::endl;
    return -1;
  }

  allocator allo(mem_size);

  std::string cmd;
  while (true) {
    std::cin >> cmd;
    if (cmd == "ALLOC") {
      int size;
      std::cin >> size;
      int p = allo.alloc(size);
      if (p != -1) {
        std::cout << "+"<< " " << p << std::endl;
      } else {
        std::cout << "-" << std::endl;
      }
    } else if (cmd == "FREE") {
      int p;
      std::cin >> p;
      if (allo.free(p)) {
        std::cout << "+" << std::endl;  
      } else {
        std::cout << "-" << std::endl;
      }
    } else if (cmd == "INFO") {
      allo.print_info();
    } else if (cmd == "MAP") {
      allo.print_map();
    } else {
      std::cout << "Illegal command" << std::endl;
    }
  }

  return 0;
}
#include <iostream>
#include "file_system.hpp"

int main (const int argc, const char *argv[]) try {
  if (argc < 3) {
    std::cout << "Usage: mkdir root path" << std::endl;
  } else {
    FS(argv[1]).find_directory(argv[2],true);
  }
  return 0;
} catch (const char * msg) {
  std::cerr << msg << std::endl;
  return 1;
} catch (const string msg) {
  std::cerr << msg << std::endl;
  return 1;
}

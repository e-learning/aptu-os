#include <iostream>
#include "file_system.hpp"

int main (const int argc, const char *argv[]) try {
  if (argc < 4) {
    std::cout << "Usage: export root fs_path host_path" << std::endl;
  } else {
    FS fs = FS(argv[1]);
    FileDescriptor fd = fs.find_descriptor(argv[2], false);

    std::ofstream out_file(argv[3], std::ios::binary | std::ios::trunc);
    if (!out_file) {
      throw "Cannot open destination file";
    }

    fs.read_data(fd, out_file);
  }
  return 0;
} catch (const char * msg) {
  std::cerr << msg << std::endl;
  return 1;
} catch (const string msg) {
  std::cerr << msg << std::endl;
  return 1;
}

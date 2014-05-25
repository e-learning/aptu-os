#include "../include/spaghetti.h"

int usage()
{
  std::cerr << "Usage: ./export <root> <fs_file> <host_file>"
            << std::endl;
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc != 4)
    return usage();

  Spaghetti::FileSystem fs(argv[1]);
  return fs._export(argv[2], argv[3]);
}




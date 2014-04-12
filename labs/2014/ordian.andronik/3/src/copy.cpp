#include "../include/spaghetti.h"

int usage()
{
  std::cerr << "Usage: ./copy <root> <src> <dst>"
            << std::endl;
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc != 4)
    return usage();

  Spaghetti::FileSystem fs(argv[1]);
  return fs._copy(argv[2], argv[3]);
}



#include "../include/spaghetti.h"

int usage()
{
  std::cerr << "Usage: ./mkdir <root> <path>"
            << std::endl;
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc != 3)
    return usage();

  Spaghetti::FileSystem fs(argv[1]);
  return fs._mkdir(argv[2]);
}




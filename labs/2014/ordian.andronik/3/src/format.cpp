#include "../include/spaghetti.h"

int usage()
{
  std::cerr << "Usage: ./format <root>" << std::endl;
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
    return usage();
  Spaghetti::FileSystem fs(argv[1]);
  return fs._format();
}


#include "../include/spaghetti.h"

int usage()
{
  std::cerr << "Usage: ./init <root>" << std::endl;
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
    return usage();
  Spaghetti::FileSystem fs(argv[1], false);
  return fs._init();
}

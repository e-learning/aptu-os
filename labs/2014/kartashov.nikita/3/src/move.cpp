#include "utils/FileSystem.h"

int usage()
{
  std::cerr << "Usage: ./move <root> <src> <dst>"
            << std::endl;
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc != 4)
    return usage();

  FileSystem fs(argv[1]);
  return fs.doMove(argv[2], argv[3]);
}




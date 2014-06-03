#include "utils/FileSystem.h"

int usage()
{
  std::cerr << "Usage: ./init <root>" << std::endl;
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
    return usage();
  FileSystem fs(argv[1], false);
  return fs.doInit();
}

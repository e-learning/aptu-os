#include <iostream>
#include <string>
#include <fstream>
#include "inode.h"
#include "fsinit.h"
#include "superblock.h"

int main(int argc, char *argv[])
{
  if (argc < 2) {
      std::cerr << "To few arguments" << std::endl;
      return 1;
    }

  std::string initRoot(argv[1]);
  std::string configPath = initRoot + "/config";
  std::ifstream configStream;
  configStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    configStream.open(configPath, std::ios_base::in);
    FsInit fsInit(initRoot);
    if (!fsInit.parseConfigure (configStream)) {
        return 1;
    }

    fsInit.init();

    configStream.close();
  }
  catch (std::ios_base::failure e) {
    std::cerr << e.what () << "Exception opening/reading/closing file" << std::endl;
    return 1;
  }

  return 0;


}

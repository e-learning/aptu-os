#include <iostream>
#include <string>
#include <fstream>
#include "fscontroller.h"


int main(int argc, char *argv[])
{
  if (argc < 4) {
      std::cerr << "To few arguments" << std::endl;
      return 1;
    }

  std::string initDir(argv[1]);
  std::string src(argv[2]);
  std::string dst(argv[3]);

  try {
    FsController controller(initDir);
    controller.mount ();
    if (!controller.copy (src, dst)) {
        std::cerr << "copy error" << std::endl;
        controller.unmount ();
        return 1;

    }
    controller.unmount ();
  }
  catch (std::ios_base::failure e) {
    std::cerr << e.what () << "Exception opening/reading/closing file" << std::endl;
    return 1;
  }

  return 0;


}

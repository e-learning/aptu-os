#include "Const.h"
#include "structures.h"
#include "utility.h"
#include "command.h"
#include <iostream>
#include <string>
#include <vector>


int exmain(int argc, char* argv[])
{
    /*
    if (argc < 4) {
        std::cout << "Not enough arguments!" << "\n" << "Use: init <root>" << std::endl;
        return ERROR_FEW_ARGS;
    }
    std::string root = argv[1];
    std::string fsfile = argv[2];
    std::string hostfile = argv[3];
    */

    std::string rootpath = "/home/novokrest/Desktop/root";
    std::string fsfile = "/p1/x";
    std::string hostfile = "/home/novokrest/Desktop/text3ex";


    Root root(rootpath);

    std::vector<std::string> vpath;
    size_t result = get_path(fsfile, vpath);
    if ((result != SUCCESS) || (vpath.size() < 1)) {
        print_error(result);
        return result;
    }

    Bitmap bitmap(root.get_bitmapCount());
    Dir & rootDir = root.get_rootDir();
    rootDir.readSelf(root);
    result = exportFile (root, bitmap, rootDir, vpath.begin(), vpath.end() - 1, hostfile);
    if (result != SUCCESS) {
        print_error(result);
        return result;
    }
    rootDir.writeSelf(root, bitmap);
    root.writeSelf();

    return SUCCESS;
}


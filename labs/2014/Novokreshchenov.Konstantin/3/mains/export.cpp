#include "Const.h"
#include "structures.h"
#include "utility.h"
#include "command.h"
#include <iostream>
#include <string>
#include <vector>


int main(int argc, char* argv[])
{
    if (argc < 4) {
        print_error(ERROR_FEW_ARGS);
        return ERROR_FEW_ARGS;
    }
    std::string rootpath = argv[1];
    std::string fsfile = argv[2];
    std::string hostfile = argv[3];

    std::vector<std::string> vpath;
    size_t result = get_path(fsfile, vpath);
    if ((result != SUCCESS) || (vpath.size() < 1)) {
        print_error(result);
        return result;
    }
    Root* root = new Root(rootpath);
    Bitmap* bitmap = new Bitmap(root);
    Dir* rootDir = root->get_rootDir();
    rootDir->readSelf(root);

    result = exportFile (root, bitmap, rootDir, vpath.begin(), vpath.end() - 1, hostfile);
    if (result != SUCCESS) {
        print_error(result);
        return result;
    }
    SaveFS(root, bitmap, rootDir);

    return SUCCESS;
}


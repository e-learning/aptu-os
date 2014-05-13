#include "Const.h"
#include "structures.h"
#include "command.h"
#include "utility.h"
#include <iostream>
#include <string>
#include <vector>


int main(int argc, char* argv[])
{
    if (argc < 3) {
        print_error(ERROR_FEW_ARGS);
        return ERROR_FEW_ARGS;
    }
    std::string rootpath = argv[1];
    std::string filepath = argv[2];

    std::vector<std::string> vpath;
    size_t pathResult = get_path(filepath, vpath);
    if (pathResult != SUCCESS) {
        print_error(pathResult);
        return pathResult;
    }

    Root* root = new Root(rootpath);
    Bitmap* bitmap = new Bitmap(root);
    Dir * rootDir = root->get_rootDir();
    rootDir->readSelf(root);

    size_t result = print_ls (rootDir, vpath.begin(), vpath.end());
    if (result != SUCCESS) {
        print_error(result);
        return result;
    }
    SaveFS(root, bitmap, rootDir);

    return SUCCESS;
}






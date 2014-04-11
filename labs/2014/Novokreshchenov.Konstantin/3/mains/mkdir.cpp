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
    std::string dirpath = argv[2];

    std::vector<std::string> vpath;

    size_t pathResult = get_path(dirpath, vpath);
    if ((pathResult != SUCCESS) || (vpath.size() < 1)) {
        print_error(pathResult);
        return pathResult;
    }

    Root root(rootpath);
    if (root.get_freeBlockscount() < 1) {
        print_error(ERROR_NOT_ENOUGH_SPACE);
        return ERROR_NOT_ENOUGH_SPACE;
    }

    Bitmap bitmap (root.get_bitmapCount());
    Dir & rootDir = root.get_rootDir();
    rootDir.readSelf(root);

    size_t result = create_dirpath(root, bitmap, rootDir, vpath.begin(), vpath.end());
    if (result != SUCCESS) {
        print_error(result);
        return result;
    }
    rootDir.writeSelf(root, bitmap);
    root.writeSelf();

    return SUCCESS;
}


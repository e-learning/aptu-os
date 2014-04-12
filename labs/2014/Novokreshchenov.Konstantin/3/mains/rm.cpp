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
    std::string record = argv[2];

    std::vector<std::string> vpath;
    size_t pathResult = get_path(record, vpath);
    if (pathResult != SUCCESS) {
        print_error(pathResult);
        return pathResult;
    }
    if (vpath.size() < 1) {
        format(rootpath);
        return SUCCESS;
    }

    Root root(rootpath);
    Bitmap bitmap (root.get_bitmapCount());
    Dir & rootDir = root.get_rootDir();
    rootDir.readSelf(root);

    size_t result = delete_rm (root, bitmap, rootDir, vpath.begin(), vpath.end() - 1);
    if (result != SUCCESS) {
        print_error(result);
        return result;
    }
    rootDir.writeSelf(root, bitmap);
    root.writeSelf();

    return SUCCESS;
}







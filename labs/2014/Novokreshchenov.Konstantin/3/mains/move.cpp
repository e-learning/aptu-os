#include "Const.h"
#include "command.h"
#include "structures.h"
#include <string>
#include <iostream>


int main(int argc, char* argv[])
{
    if (argc < 4) {
        print_error(ERROR_FEW_ARGS);
        return ERROR_FEW_ARGS;
    }
    std::string rootpath = argv[1];
    std::string srcdirPath = argv[2];
    std::string dstdirPath = argv[3];

    std::vector<std::string> vpathSrc;
    std::vector<std::string> vpathDst;

    //checker
    size_t pathResultSrc = get_path(srcdirPath, vpathSrc);
    if ((vpathSrc.size() <  1) || (pathResultSrc != SUCCESS)) {
        print_error(ERROR_INCORRECT_PATH);
        return ERROR_INCORRECT_PATH;
    }
    size_t pathResultDst = get_path(dstdirPath, vpathDst);
    if (pathResultDst != SUCCESS) {
        print_error(pathResultDst);
        return pathResultDst;
    }
    if (src_isPartOf_dst(vpathSrc, vpathDst)) {
        print_error(ERROR_INCORRECT_PATH);
        return ERROR_INCORRECT_PATH;
    }
    //end of checker

    Root root(rootpath);
    Bitmap bitmap (root.get_bitmapCount());
    Dir & rootDir = root.get_rootDir();
    rootDir.readSelf(root);

    size_t result = move(root, rootDir, bitmap,
                         rootDir, vpathSrc.begin(), vpathSrc.end() - 1,
                         vpathDst, true);
    if (result != SUCCESS) {
        print_error(result);
        return result;
    }

    rootDir.writeSelf(root, bitmap);
    root.writeSelf();

    return SUCCESS;
}

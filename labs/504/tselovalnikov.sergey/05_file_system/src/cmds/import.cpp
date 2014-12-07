#include <vector>
#include <iostream>
#include "../Context.h"
#include "../utils/StringUtils.h"
#include "../FileSystem.h"

using namespace std;

int main(int argc, char *argv[]) {
    string path = argv[1];
    Context *ctx = Context::readFromDirectory(path);

    FileSystem fs(ctx);
    fs.import_file(argv[2], argv[3]);

    return 0;
}
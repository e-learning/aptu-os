#include <vector>
#include <iostream>
#include "../Context.h"
#include "../FileSystem.h"

using namespace std;

int main(int argc, char* argv[]) {
    string path = argv[1];
    Context* ctx = Context::readFromDirectory(path);

    FileSystem::init(ctx);
    return 0;
}
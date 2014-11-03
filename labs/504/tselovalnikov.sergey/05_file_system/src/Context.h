#include <fstream>

#pragma once

using namespace std;

class Context {
public:
    int block_count = 0;
    int block_size = 0;
    string root;

    static Context* readFromDirectory(string path) {
        string root = path;
        ifstream ifs(root + "/config");
        int blc, bls;
        ifs >> bls >> blc;
        Context* ctx = new Context();
        ctx->block_size = bls;
        ctx->block_count = blc;
        ctx->root = root;
        return ctx;
    }
};
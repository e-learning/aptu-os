#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class BinReader {
public:
    BinReader(ifstream& is) : ifs(is) {
    }

    int readInt() {
        int value = 0;
        ifs.read((char*)&value, sizeof(int));
        return value;
    }

    char* readStr(int len) {
        char* str = new char[len + 1];
        str[len] = '\0';
        ifs.read(str, len);
        return str;
    }

    bool readBool() {
        bool is_dir = false;
        ifs.read((char*)&is_dir, sizeof(bool));
        return is_dir;
    }

private:
    ifstream& ifs;
};


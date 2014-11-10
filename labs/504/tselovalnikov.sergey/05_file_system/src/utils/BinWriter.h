#pragma once

#include <iostream>

using namespace std;

class BinWriter {
public:
    BinWriter(ofstream& os) : ofs(os) {
    }

    void putInt(int value) {
        ofs.write((char*)&value, sizeof(int));
    }

    void putStr(string str, int len) {
        ofs.write(str.c_str(), len);
    }

    void putBool(bool b) {
        ofs.write((char*)&b, sizeof(bool));
    }

private:
    ofstream& ofs;
};


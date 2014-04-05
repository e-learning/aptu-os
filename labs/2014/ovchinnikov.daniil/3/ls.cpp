#include <iostream>
#include "fs.h"
#include <ctime>
using namespace std;

template<typename T>
ostream & operator<<(ostream & out_, const vector<T> & v){
    for (auto it = v.begin(); it!=v.end(); ++it) {
       out_ << *it;
    }
    return out_;
}

ostream & operator <<(ostream & out_, const FileDescriptor & fd) {
    out_ << fd.name << '\t';
    if  (fd.directory) {
        out_ << "directory" << endl;
    } else {
        out_ << fd.number_of_blocks << " blocks";
        out_<< '\t' << ctime(&fd.time);
    }
    return out_;
}


int main (const int argc, const char *argv[]) try {
    if (argc < 3) {
        std::cout << "Usage: mkdir root path" << std::endl;
        return 0;
    } else {
        FS f(argv[1]);
        FileDescriptor fd = f.find(argv[2]);
        if (fd.directory) {
            cout << f.list(fd);
        } else {
            cout << fd;
        }
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
}


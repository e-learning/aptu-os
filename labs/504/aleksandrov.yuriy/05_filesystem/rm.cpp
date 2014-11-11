#include <iostream>
#include "meta_fs.h"

using namespace std;


int main (int argc, char *argv[]) {
    try
    {
        if (argc != 3)
        {
            cout << "Usage: rm root path" << endl;
            return -2;
        }
        else
        {
            Meta_FS fs = Meta_FS(argv[1]);
            Inode inode = fs.get_inode(argv[2], false);
            fs.rm(inode);
        }
        return 0;
    }
    catch (const FSException exc)
    {
        cerr << exc.what() << endl;
        return -1;
    }
}

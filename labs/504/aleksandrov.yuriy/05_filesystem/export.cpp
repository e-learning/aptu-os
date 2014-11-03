#include <iostream>
#include "meta_fs.h"

using namespace std;


int main (int argc, char *argv[]) {
    try
    {
        if (argc != 4)
        {
            cout << "Usage: export root fs_file host_file" << endl;
            return -2;
        }
        else
        {
            Meta_FS fs = Meta_FS(argv[1]);
            Inode inode = fs.get_inode(argv[2], false);
            ofstream outfile(argv[3], ios::binary | ios::trunc);
            if (!outfile)
            {
                throw FSException("Can't open file");
            }
            fs.read_data(inode, outfile);
        }
        return 0;
    }
    catch (const FSException exc)
    {
        cerr << exc.what() << endl;
        return -1;
    }
}

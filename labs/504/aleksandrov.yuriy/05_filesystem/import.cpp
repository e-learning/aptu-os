#include <iostream>
#include "meta_fs.h"
#include <string>

using namespace std;


int main (int argc, char *argv[]) {
    try
    {
        if (argc != 4)
        {
            cout << "Usage: import root host_file fs_file" << endl;
            return -2;
        }
        else
        {
            Meta_FS fs(argv[1]);
            ifstream source(argv[2], ios::binary);
            if (!source)
            {
                throw FSException("Can't open file to import");
            }
            fs.check_availability(source);
            Inode inode = fs.get_inode(argv[3], source);
            if (inode.is_dir)
            {
                throw FSException(string("'") + inode.name + "' is a directory");
            }
            fs.write_data(inode, source);
        }
        return 0;
    }
    catch (const FSException exc)
    {
        cerr << exc.what() << endl;
        return -1;
    }
}

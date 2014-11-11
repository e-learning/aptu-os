#include <iostream>
#include "meta_fs.h"

using namespace std;


int main (int argc, char *argv[]) {
    try
    {
        if (argc != 4)
        {
            cout << "Usage: mv root stc dst" << endl;
            return -2;
        }
        else
        {
            Meta_FS fs = Meta_FS(argv[1]);
            Inode src_inode = fs.get_inode(argv[2], false);
            Inode dst_inode = fs.get_inode(argv[3], true, src_inode.is_dir);
            fs.mv(src_inode, dst_inode);
        }
        return 0;
    }
    catch (const FSException exc)
    {
        cerr << exc.what() << endl;
        return -1;
    }
}

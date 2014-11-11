#include <iostream>
#include "meta_fs.h"

using namespace std;


int main (int argc, char *argv[]) {
    try
    {
        if (argc != 3)
        {
            cout << "Usage: ls root path" << endl;
            return -2;
        }
        else
        {
            Meta_FS fs(argv[1]);
            Inode inode = fs.get_inode(argv[2], false);
            if (inode.is_dir)
            {
                vector<Inode> inodes = fs.ls(inode);
                for (vector<Inode>::iterator iter = inodes.begin(); iter != inodes.end(); ++iter)
                {
                    cout << (*iter).name << ((*iter).is_dir ? " d" : " f") << '\n';
                }
                cout << flush;
            }
            else
            {
                cout << argv[2] << ' ' << inode.blocks_num << endl;
            }
        }
        return 0;
    }
    catch (const FSException exc)
    {
        cerr << exc.what() << endl;
        return -1;
    }
}

#include <iostream>
#include <unistd.h>
#include "meta_fs.h"


int main (int argc, char *argv[]) {
    try
    {
        if (argc != 2)
        {
            cout << "Usage: init root" << endl;
            return -2;
        }
        else
        {
            Meta_FS fs(argv[1]);
            Config config = fs.get_config();
            for (int i = 0; i < config.blocks_num; ++i)
            {
                const string block_name = fs.get_block_path(i);
                ofstream(block_name, ios::trunc).close();
                if (truncate(block_name.c_str(), config.block_size))
                {
                    throw FSException("Can't create blocks");
                }
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

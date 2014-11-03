#include <iostream>
#include "meta_fs.h"


int main (int argc, char *argv[]) {
    try
    {
        if (argc != 2)
        {
            cout << "Usage: format root" << endl;
            return -2;
        }
        else
        {
            Meta_FS(argv[1]).format();
        }
        return 0;
    }
    catch (const FSException exc)
    {
        cerr << exc.what() << endl;
        return -1;
    }
}

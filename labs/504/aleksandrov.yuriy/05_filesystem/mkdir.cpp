#include <iostream>
#include "meta_fs.h"

using namespace std;


int main (int argc, char *argv[]) {
    try
    {
        if (argc != 3)
        {
            cout << "Usage: mkdir root path" << endl;
            return -2;
        }
        else
        {
            Meta_FS(argv[1]).get_directory(argv[2], true);
        }
        return 0;
    }
    catch (const FSException exc)
    {
        cerr << exc.what() << endl;
        return -1;
    }
}

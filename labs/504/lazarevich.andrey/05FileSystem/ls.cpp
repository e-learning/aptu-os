#include "myfs.h"

int main(int argc, char **argv)
{
    using namespace std;
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " <root_directory> <path>" << endl;
        return -1;
    }
    else
    {
        MyFS fs(argv[1], false);
        cout << fs.ls(argv[2]) << endl;
    }
    return 0;
}

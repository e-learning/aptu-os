#include "myfs.h"

int main(int argc, char **argv)
{
    using namespace std;
    if (argc != 4)
    {
        cout << "Usage: " << argv[0] << " <root_directory> <host_file> <fs_file>" << endl;
        return -1;
    }
    else
    {
        MyFS fs(argv[1], false);
        return fs.import_file(argv[2], argv[3]);
    }
}

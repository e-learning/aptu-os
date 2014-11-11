#include "myfs.h"

int main(int argc, char **argv)
{
    using namespace std;
    if (argc != 4)
    {
        cout << "Usage: " << argv[0] << " <root_directory> <source> <dest>" << endl;
        return -1;
    }
    else
    {
        MyFS fs(argv[1], false);
        return fs.move(argv[2], argv[3]);
    }
}

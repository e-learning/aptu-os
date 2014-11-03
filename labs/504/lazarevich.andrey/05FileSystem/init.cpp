#include "myfs.h"

int main(int argc, char **argv)
{
    using namespace std;

    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <root_directory>" << endl;
        return -1;
    }
    else
    {
        MyFS fs(argv[1], true);
        return fs.init();
    }
}

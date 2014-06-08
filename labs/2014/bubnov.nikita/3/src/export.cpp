#include "lib/file_system.h"

int usage()
{
    std::cerr << "Usage: ./export <root> <fs_file> <host_file>" << std::endl;
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
        return usage();

    FileSystem fs(argv[1]);
    return fs.doExport(argv[2], argv[3]);
}




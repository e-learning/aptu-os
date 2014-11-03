#include "fs.cpp"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        return -1;
    }

    FS fs(argv[1]);

    return fs.format();
}
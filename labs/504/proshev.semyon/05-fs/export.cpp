#include "fs.cpp"

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        return -1;
    }
    else
    {
        FS fs(argv[1]);

        return fs.exp(argv[2], argv[3]);
    }
}
#include "copy.h"


int main(int argc, char **argv)
{
    if (copy_main(argc, argv) != 0) {
        return EXIT_FAILURE;
    }
    return rm_main(argc - 1, argv);
}

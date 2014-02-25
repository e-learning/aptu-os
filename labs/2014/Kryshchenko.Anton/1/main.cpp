#include "interpreter.h"

int main()
{
    info_command();
    do {


    } while ( exec_command( read_command() ) );

    return 0;
}


#include <signal.h>
#include "sh.h"


int main(int argc, char ** argv)
{
    signal(SIGINT, signal_handler);
    run();
    return 0;
}
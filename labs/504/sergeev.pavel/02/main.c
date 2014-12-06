#include <stdio.h>

#include "scheduler.h"

int main(int argc, char** argv)
{
    load_tasks(stdin);
    run_scheduler(stdout);
    return 0;
}
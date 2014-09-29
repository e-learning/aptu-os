#include <iostream>
#include <string>
#include "scheduler.h"

using namespace std;

int Process::quant = 0;

int main()
{
    int quant = 0;
    cin >> quant;
    Scheduler scheduler(quant);
    string inputline;
    while (getline(cin, inputline))
    {
        if (inputline.length() == 0)
            continue;
        scheduler.add(inputline);
    }
    scheduler.proceed();
    return 0;
}


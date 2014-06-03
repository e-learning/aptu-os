#include <iostream>
#include <vector>
#include <cstdio>

#include "process.h"
#include "scheduler.h"

using std::cin;
using std::cout;
using std::endl;

//#define DEBUG

int main() {
#ifdef DEBUG
    std::freopen("../OsProcessScheduler/test3", "r", stdin);
#endif
    size_t time_slot;
    cin >> time_slot;
    std::vector<Process> processes;
    std::string buf;
    cin.get();
    while(std::getline(cin, buf)) {
        processes.push_back(Process::FromString(buf));
    }
    Scheduler scheduler(time_slot, processes);
    scheduler.Start();
    return 0;
}

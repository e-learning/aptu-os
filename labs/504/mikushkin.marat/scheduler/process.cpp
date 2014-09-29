#include "process.h"

Process::Process(std::string line) {
    remainingTime = 0;
    std::stringstream ss(line);

    ss >> name;
    ss >> start >> end;

    int ioOperationStart, ioOperationEnd;
    while(ss >> ioOperationStart >> ioOperationEnd) {
        ioOperations.push(std::pair<int,int>(ioOperationStart, ioOperationEnd));
    }
}

int Process::timeToRun() {
    if(!ioOperations.empty()) {
        return ioOperations.front().first - remainingTime;
    } else {
        return end - remainingTime;
    }
}

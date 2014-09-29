#include "process.h"

Process::Process(std::string line) {
    remainingTime = 0;
    std::stringstream ss(line);

    ss >> name;
    ss >> start >> end;

    int io_start, io_len;
    while(ss >> io_start >> io_len) {
        ioOperations.push(std::pair<int,int>(io_start, io_len));
    }
}

int Process::timeToRun() {
    if(!ioOperations.empty()) {
        return ioOperations.front().first - remainingTime;
    } else {
        return end - remainingTime;
    }
}

#include "process.h"

Process::Process(std::string line) {
    time_runned = 0;
    std::stringstream ss(line);

    ss >> name;
    ss >> start >> end;

    int io_start, io_len;
    while(ss >> io_start >> io_len) {
        iops.push(std::pair<int,int>(io_start, io_len));
    }
}

int Process::timeToRun() {
    if(!iops.empty()) {
        return iops.front().first - time_runned;
    } else {
        return end - time_runned;
    }
}

#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <sstream>

class Process {
public:
    std::string name;
    int start, end;
    std::queue<std::pair<int, int> > iops;
    int time_runned;

    Process(std::string line);
    int timeToRun();
};

#endif // PROCESS_H

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
    int start;
    int end;
    int remainingTime;
    std::queue<std::pair<int, int> > ioOperations;

    Process(std::string line);
    int timeToRun();
};

#endif // PROCESS_H

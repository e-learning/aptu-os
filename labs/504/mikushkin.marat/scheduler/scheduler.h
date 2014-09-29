#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

struct StartTimeCmp {
    bool operator() (Process & first, Process & second) {
        return first.start > second.start;
    }
};

struct TimeToRunCmp {
    bool operator() (Process & first, Process & second) {
        return first.timeToRun() > second.timeToRun();
    }
};

class Scheduler {
public:
    int quantLength;
    int currentTime;
    std::priority_queue<Process, std::vector<Process>, StartTimeCmp> waitingTasks;
    std::priority_queue<Process, std::vector<Process>, TimeToRunCmp> readyTasks;

    Scheduler();
    void processTask(Process & curTask);
    void makeMove();
    void readInput();

};

#endif // SCHEDULER_H

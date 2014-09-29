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
    int processorUnit;
    int currentTime;
    std::priority_queue<Process, std::vector<Process>, StartTimeCmp> waitingTasks;
    std::priority_queue<Process, std::vector<Process>, TimeToRunCmp> readyTasks;

    Scheduler();
    void processTask(Process & curTask);
    void makeMove();
    void init();

};

#endif // SCHEDULER_H

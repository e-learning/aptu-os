#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

class Scheduler {
public:
    struct ComparatorByStartTime {
        bool operator() (Process & first, Process & second) {
            return first.start > second.start;
        }
    };

    struct ComparatorByTimeToRun {
        bool operator() (Process & first, Process & second) {
            return first.timeToRun() > second.timeToRun();
        }
    };

    int processorUnit;
    int currentTime;
    std::priority_queue<Process, std::vector<Process>, ComparatorByStartTime> waitingTasks;
    std::priority_queue<Process, std::vector<Process>, ComparatorByTimeToRun> readyTasks;

    Scheduler();
    void sendToProcessor(Process & currentProcess);
    void makeMove();
    void init();

};

#endif // SCHEDULER_H
